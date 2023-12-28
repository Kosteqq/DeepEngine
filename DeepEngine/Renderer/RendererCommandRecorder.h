#pragma once
#include "MainRenderPass.h"
#include "TriangleRenderer.h"
#include "Vulkan/Semaphore.h"
#include "Vulkan/VulkanPCH.h"
#include "Vulkan/CommandBuffer.h"
#include "Vulkan/CommandPool.h"
#include "Vulkan/Fence.h"
#include "Vulkan/GraphicsPipeline.h"
#include "Vulkan/RenderPass.h"

namespace DeepEngine::Renderer
{

    class RendererCommandRecorder
    {
    public:
        RendererCommandRecorder() = default;
        RendererCommandRecorder(Vulkan::VulkanInstance* p_vulkanInstance, const Vulkan::VulkanInstance::QueueInstance* p_mainGraphicsQueue)
                : _vulkanInstance(p_vulkanInstance), _mainGraphicsQueue(p_mainGraphicsQueue)
        {
            uint32_t cmdPoolFlags = 0;
            cmdPoolFlags |= Vulkan::CommandPoolFlag::RESET_COMMAND_BUFFER;
            
            _commandPool = new Vulkan::CommandPool(_mainGraphicsQueue, (Vulkan::CommandPoolFlag)cmdPoolFlags);
            _vulkanInstance->InitializeSubController(_commandPool);
            
            _commandBuffer = new Vulkan::CommandBuffer(_commandPool, false);
            _commandPool->InitializeSubController(_commandBuffer);
        }
        
        void Terminate()
        {
            _commandPool->Terminate();
        }
        
        void RecordBuffer(glm::vec4 p_clearColor, uint32_t p_frameBufferIndex,
            MainRenderPass* p_renderPass, const std::vector<TriangleRenderer>& p_renderers )
        {
            VkCommandBufferBeginInfo beginInfo { };
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0;
            beginInfo.pInheritanceInfo = nullptr;

            const auto status = vkBeginCommandBuffer(_commandBuffer->GetVkCommandBuffer(), &beginInfo);
            if (status != VK_SUCCESS)
            {
                VULKAN_ERR("Failed to create begin command buffer with returned result {}", string_VkResult(status));
                return;
            }

            VkClearValue clearColor = {
                { p_clearColor.r, p_clearColor.g, p_clearColor.b, p_clearColor.a }
            };

            VkExtent2D renderAreaExtent(_vulkanInstance->GetFrameBufferSize().x,
                    _vulkanInstance->GetFrameBufferSize().y);

            VkRenderPassBeginInfo renderPassInfo { };
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = p_renderers[0].GetGraphicsPipeline()->GetVkRenderPass();
            renderPassInfo.framebuffer = p_renderPass->GetSwapchainImageVkFramebuffer(p_frameBufferIndex);
            renderPassInfo.clearValueCount = 1;
            renderPassInfo.pClearValues = &clearColor;
            renderPassInfo.renderArea.offset = { 0, 0 };
            renderPassInfo.renderArea.extent = renderAreaExtent;

            vkCmdBeginRenderPass(_commandBuffer->GetVkCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            VkViewport viewport;
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = _vulkanInstance->GetFrameBufferSize().x;
            viewport.height = _vulkanInstance->GetFrameBufferSize().y;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            
            vkCmdSetViewport(_commandBuffer->GetVkCommandBuffer(), 0, 1, &viewport);

            VkRect2D scissor;
            scissor.offset = {0, 0};
            scissor.extent = renderAreaExtent;
            
            vkCmdSetScissor(_commandBuffer->GetVkCommandBuffer(), 0, 1, &scissor);

            for (auto& renderer : p_renderers)
            {
                vkCmdBindPipeline(_commandBuffer->GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS,
                                renderer.GetGraphicsPipeline()->GetVkPipeline());

                vkCmdDraw(_commandBuffer->GetVkCommandBuffer(), 3, 1, 0, 0);
            }
            
            vkCmdEndRenderPass(_commandBuffer->GetVkCommandBuffer());

            const auto result = vkEndCommandBuffer(_commandBuffer->GetVkCommandBuffer());
            if (result != VK_SUCCESS)
            {
                VULKAN_ERR("Failed to record command buffer with returned result {}", string_VkResult(status));
                return;
            }
        }
        
        void SubmitBuffer(const Vulkan::Fence* p_finishFence,
            const std::vector<const Vulkan::Semaphore*>& p_waitSemaphores,
            const std::vector<const Vulkan::Semaphore*>& p_finishSemaphores,
            const std::vector<const Vulkan::CommandBuffer*>& p_additionalCommandBuffers)
        {
            std::vector<VkSemaphore> waitSemaphores(p_waitSemaphores.size());
            for (uint32_t i = 0; i < waitSemaphores.size(); i++)
            {
                waitSemaphores[i] = p_waitSemaphores[i]->GetVkSemaphore();
            }

            std::vector<VkSemaphore> finishSubmitSemaphores(p_finishSemaphores.size());
            for (uint32_t i = 0; i < finishSubmitSemaphores.size(); i++)
            {
                finishSubmitSemaphores[i] = p_finishSemaphores[i]->GetVkSemaphore();
            }

            std::vector<VkCommandBuffer> commandBuffers(p_additionalCommandBuffers.size() + 1);
            commandBuffers[0] = _commandBuffer->GetVkCommandBuffer();
            
            for (uint32_t i = 0; i < p_additionalCommandBuffers.size(); i++)
            {
                commandBuffers[i + 1] = p_additionalCommandBuffers[i]->GetVkCommandBuffer();
            }

            VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

            VkSubmitInfo submitInfo { };
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
            submitInfo.pWaitSemaphores = waitSemaphores.data();
            submitInfo.pWaitDstStageMask = waitStages;
            submitInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
            submitInfo.pCommandBuffers = commandBuffers.data();
            submitInfo.signalSemaphoreCount = static_cast<uint32_t>(finishSubmitSemaphores.size());
            submitInfo.pSignalSemaphores = finishSubmitSemaphores.data();

            VkResult result = vkQueueSubmit(_mainGraphicsQueue->Queue, 1, &submitInfo, p_finishFence->GetVkFence());

            if (result != VK_SUCCESS)
            {
                VULKAN_ERR("Failed to submit queue!! Returned result: \"{}\"", string_VkResult(result));
            }
        }

    private:
        const Vulkan::VulkanInstance::QueueInstance* _mainGraphicsQueue;
        Vulkan::VulkanInstance* _vulkanInstance;

        Vulkan::CommandBuffer* _commandBuffer;
        Vulkan::CommandPool* _commandPool;
    };
    
}
