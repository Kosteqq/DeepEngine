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

namespace DeepEngine::Engine::Renderer
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

            _commandPool = Vulkan::Factory::SubFactory<Vulkan::CommandPool>::Create(_mainGraphicsQueue, (Vulkan::CommandPoolFlag)cmdPoolFlags);
            _commandBuffer = Vulkan::Factory::SubFactory<Vulkan::CommandBuffer>::Create(_commandPool);
        }
        
        void RecordBuffer(glm::vec4 p_clearColor, uint32_t p_frameBufferIndex,
            std::shared_ptr<MainRenderPassController>& p_renderPass,
            const std::vector<TriangleRenderer>& p_renderers, 
            VkImage p_renderPassOutputImage)
        {
            VkCommandBufferBeginInfo beginInfo { };
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0;
            beginInfo.pInheritanceInfo = nullptr;

            const auto status = vkBeginCommandBuffer(_commandBuffer->GetHandler(), &beginInfo);
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
            renderPassInfo.renderPass = p_renderPass->GetRenderPassHandler();
            renderPassInfo.framebuffer = p_renderPass->GetFrameBufferHandler(p_frameBufferIndex);
            renderPassInfo.clearValueCount = 1;
            renderPassInfo.pClearValues = &clearColor;
            renderPassInfo.renderArea.offset = { 0, 0 };
            renderPassInfo.renderArea.extent = renderAreaExtent;

            vkCmdBeginRenderPass(_commandBuffer->GetHandler(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            VkViewport viewport;
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = _vulkanInstance->GetFrameBufferSize().x;
            viewport.height = _vulkanInstance->GetFrameBufferSize().y;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            
            vkCmdSetViewport(_commandBuffer->GetHandler(), 0, 1, &viewport);

            VkRect2D scissor;
            scissor.offset = {0, 0};
            scissor.extent = renderAreaExtent;
            
            vkCmdSetScissor(_commandBuffer->GetHandler(), 0, 1, &scissor);

            Vulkan::GraphicsPipeline2* currentPipeline = nullptr;
            
            for (auto& renderer : p_renderers)
            {
                if (currentPipeline != renderer.GetGraphicsPipeline().get())
                {
                    currentPipeline = renderer.GetGraphicsPipeline().get();
                    vkCmdBindPipeline(_commandBuffer->GetHandler(), VK_PIPELINE_BIND_POINT_GRAPHICS, currentPipeline->GetHandler());
                }
                
                vkCmdDraw(_commandBuffer->GetHandler(), 3, 1, 0, 0);
            }
            
            vkCmdEndRenderPass(_commandBuffer->GetHandler());

            

            VkImageSubresourceRange aspect { };
            aspect.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            aspect.baseMipLevel = 0;
            aspect.levelCount = 1;
            aspect.baseArrayLayer = 0;
            aspect.layerCount = 1;

            VkImageMemoryBarrier imageBarrier = {};
            imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            imageBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            imageBarrier.newLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
            imageBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            imageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            imageBarrier.image = p_renderPassOutputImage;
            imageBarrier.subresourceRange = aspect;

            vkCmdPipelineBarrier(_commandBuffer->GetHandler(),
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                0,
                0,
                nullptr,
                0,
                nullptr,
                1,
                &imageBarrier);

            const auto result = vkEndCommandBuffer(_commandBuffer->GetHandler());
            if (result != VK_SUCCESS)
            {
                VULKAN_ERR("Failed to record command buffer with returned result {}", string_VkResult(status));
                return;
            }
        }
        
        void SubmitBuffer(const Vulkan::Ref<Vulkan::Fence>& p_finishFence,
            const std::vector<Vulkan::Ref<Vulkan::Semaphore>>& p_waitSemaphores,
            const std::vector<Vulkan::Ref<Vulkan::Semaphore>>& p_finishSemaphores,
            const Vulkan::Ref<Vulkan::CommandBuffer>& p_imGuiComamandBuffer)
        {
            std::vector<VkSemaphore> waitSemaphores(p_waitSemaphores.size());
            for (uint32_t i = 0; i < waitSemaphores.size(); i++)
            {
                waitSemaphores[i] = p_waitSemaphores[i]->GetHandler();
            }

            std::vector<VkSemaphore> finishSubmitSemaphores(p_finishSemaphores.size());
            for (uint32_t i = 0; i < finishSubmitSemaphores.size(); i++)
            {
                finishSubmitSemaphores[i] = p_finishSemaphores[i]->GetHandler();
            }

            std::array<VkCommandBuffer, 2> commandBuffers{
                _commandBuffer->GetHandler(),
                p_imGuiComamandBuffer->GetHandler(),
            };

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

            VkResult result = vkQueueSubmit(_mainGraphicsQueue->Queue, 1, &submitInfo, p_finishFence->GetHandler());

            if (result != VK_SUCCESS)
            {
                VULKAN_ERR("Failed to submit queue!! Returned result: \"{}\"", string_VkResult(result));
            }
        }

    private:
        const Vulkan::VulkanInstance::QueueInstance* _mainGraphicsQueue;
        Vulkan::VulkanInstance* _vulkanInstance;

        Vulkan::Ref<Vulkan::CommandBuffer> _commandBuffer;
        Vulkan::Ref<Vulkan::CommandPool> _commandPool;
    };
    
}
