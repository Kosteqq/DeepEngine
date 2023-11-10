#pragma once
#include <memory>
#include <valarray>
#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>

#include "VulkanLogicalLayer.h"
#include "VulkanPipelineLayout.h"
#include "VulkanRenderPass.h"

namespace DeepEngine::Renderer
{
    
    class VulkanCommandPool
    {
    public:
        VulkanCommandPool(std::shared_ptr<Debug::Logger> p_logger, VulkanLogicalLayer* p_logicalLayer)
            : _logger(p_logger), _logicalLayer(p_logicalLayer)
        {
            
        }

        ~VulkanCommandPool()
        {
            Terminate();
        }

        bool Init()
        {
            VkCommandPoolCreateInfo poolInfo { };
            poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            poolInfo.queueFamilyIndex = _logicalLayer->GetQueuesFamilies()[0]; // for now only use single queue

            auto status = vkCreateCommandPool(_logicalLayer->GetLogicalDevice(), &poolInfo, nullptr, &_commandPool);

            if (status != VK_SUCCESS)
            {
                LOG_ERR(_logger, "Failed to create command pool with returned result {}", string_VkResult(status));
                return false;
            }
            
            VkCommandBufferAllocateInfo allocInfo { };
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.commandPool = _commandPool;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandBufferCount = 1;

            status = vkAllocateCommandBuffers(_logicalLayer->GetLogicalDevice(), &allocInfo, &_commandBuffer);

            if (status != VK_SUCCESS)
            {
                LOG_ERR(_logger, "Failed to create command buffer with returned result {}", string_VkResult(status));
                return false;
            }

            VkSemaphoreCreateInfo semaphoreCreateInfo { };
            semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            status = vkCreateSemaphore(_logicalLayer->GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &_imageAvailableSemaphore);
            if (status != VK_SUCCESS)
            {
                LOG_ERR(_logger, "Failed to create semaphore with returned result {}", string_VkResult(status));
                return false;
            }

            status = vkCreateSemaphore(_logicalLayer->GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &_renderFinishedSemaphore);
            if (status != VK_SUCCESS)
            {
                LOG_ERR(_logger, "Failed to create semaphore with returned result {}", string_VkResult(status));
                return false;
            }

            VkFenceCreateInfo fenceCreateInfo { };
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            status = vkCreateFence(_logicalLayer->GetLogicalDevice(), &fenceCreateInfo, nullptr, &_inFlightFence);
            if (status != VK_SUCCESS)
            {
                LOG_ERR(_logger, "Failed to create fence with returned result {}", string_VkResult(status));
                return false;
            }
            
            return true;
        }

        void Terminate()
        {
            vkDestroySemaphore(_logicalLayer->GetLogicalDevice(), _imageAvailableSemaphore, nullptr);
            vkDestroySemaphore(_logicalLayer->GetLogicalDevice(), _renderFinishedSemaphore, nullptr);
            vkDestroyFence(_logicalLayer->GetLogicalDevice(), _inFlightFence, nullptr);
            vkDestroyCommandPool(_logicalLayer->GetLogicalDevice(), _commandPool, nullptr);
        }
        
        bool RecordCommandBuffer(VkFramebuffer p_frameBuffer, VulkanRenderPass* p_renderPass,
            VulkanSwapChain* p_swapChain, VulkanPipeline* p_pipeline, VertexBuffer* p_vertexBuffer)
        {
            VkCommandBufferBeginInfo beginInfo { };
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0;
            beginInfo.pInheritanceInfo = nullptr;

            const auto status = vkBeginCommandBuffer(_commandBuffer, &beginInfo);
            if (status != VK_SUCCESS)
            {
                LOG_ERR(_logger, "Failed to create begin command buffer with returned result {}", string_VkResult(status));
                return false;
            }

            VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}}; 

            VkRenderPassBeginInfo renderPassInfo { };
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = p_renderPass->GetVulkanRenderPass();
            renderPassInfo.framebuffer = p_frameBuffer;
            renderPassInfo.clearValueCount = 1;
            renderPassInfo.pClearValues= &clearColor;
            renderPassInfo.renderArea.offset = { 0,0 };
            renderPassInfo.renderArea.extent = p_swapChain->GetExtent();
            
            vkCmdBeginRenderPass(_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, p_pipeline->GetPipeline());

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = static_cast<float>(p_swapChain->GetExtent().width);
            viewport.height = static_cast<float>(p_swapChain->GetExtent().height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            vkCmdSetViewport(_commandBuffer, 0, 1, &viewport);

            VkRect2D scissor{};
            scissor.offset = {0, 0};
            scissor.extent = p_swapChain->GetExtent();
            vkCmdSetScissor(_commandBuffer, 0, 1, &scissor);

            VkBuffer vertexBuffers[] = { p_vertexBuffer->GetBuffer() };
            VkDeviceSize offsets[] = { 0 };
            vkCmdBindVertexBuffers(_commandBuffer, 0, 1, vertexBuffers, offsets);

            vkCmdDraw(_commandBuffer, p_vertexBuffer->GetVertexCount(), 1, 0, 0);
            vkCmdEndRenderPass(_commandBuffer);

            const auto result = vkEndCommandBuffer(_commandBuffer);
            if (result != VK_SUCCESS)
            {
                LOG_ERR(_logger, "Failed to record command buffer with returned result {}", string_VkResult(status));
                return false;
            }
            
            return true;
        }

        void CopyBuffer(VkBuffer p_src, VkBuffer p_dst, VkDeviceSize p_size, VkQueue p_queue)
        {
            VkCommandBufferAllocateInfo allocInfo { };
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandPool = _commandPool;
            allocInfo.commandBufferCount = 1;

            VkCommandBuffer commandBuffer;
            vkAllocateCommandBuffers(_logicalLayer->GetLogicalDevice(), &allocInfo, &commandBuffer);

            VkCommandBufferBeginInfo beginInfo { };
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            vkBeginCommandBuffer(commandBuffer, &beginInfo);

            VkBufferCopy copyRegion { };
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = 0;
            copyRegion.size = p_size;
            vkCmdCopyBuffer(commandBuffer, p_src, p_dst, 1, &copyRegion);

            vkEndCommandBuffer(commandBuffer);

            VkSubmitInfo submitInfo { };
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffer;

            vkQueueSubmit(p_queue, 1, &submitInfo, VK_NULL_HANDLE);
            vkQueueWaitIdle(p_queue);

            vkFreeCommandBuffers(_logicalLayer->GetLogicalDevice(), _commandPool, 1, &commandBuffer);
        }

        VkSemaphore GetImageAvailableSemaphore() const
        { return _imageAvailableSemaphore; }
        VkSemaphore GetRenderFinishedSemaphore() const
        { return _renderFinishedSemaphore; }
        VkFence GetInFlightFence() const
        { return _inFlightFence; }
        VkCommandBuffer GetCommandBuffer() const
        { return _commandBuffer; }

    private:
        const std::shared_ptr<DeepEngine::Debug::Logger> _logger;
        const VulkanLogicalLayer* _logicalLayer;

        VkSemaphore _imageAvailableSemaphore;
        VkSemaphore _renderFinishedSemaphore;
        VkFence _inFlightFence;
        
        VkCommandPool _commandPool;
        VkCommandBuffer _commandBuffer;
    };
    
}
