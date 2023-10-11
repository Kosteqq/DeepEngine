#pragma once
#include <memory>

#define NOMINMAX
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include "VulkanSwapChain.h"
#include "Architecture/EngineSystem.h"
#include "Debugs/Logger.h"

namespace DeepEngine::Renderer
{

    class VulkanRenderPass
    {
    public:
        VulkanRenderPass(std::shared_ptr<Debug::Logger> p_logger, const VulkanLogicalLayer* p_physicalLayer,
            const VulkanSwapChain* p_swapChain)
            : _logger(p_logger), _logicalLayer(p_physicalLayer), _swapChain(p_swapChain)
        { }

        bool Initialize()
        {
            VkAttachmentDescription colorAttachment { };
            colorAttachment.format = _swapChain->GetImageFormat();
            colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            VkAttachmentReference colorAttachmentRef { };
            colorAttachmentRef.attachment = 0;
            colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkSubpassDescription subpass { };
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &colorAttachmentRef;

            VkRenderPassCreateInfo renderPassCreateInfo { };
            renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassCreateInfo.attachmentCount = 1;
            renderPassCreateInfo.pAttachments = &colorAttachment;
            renderPassCreateInfo.subpassCount = 1;
            renderPassCreateInfo.pSubpasses = &subpass;

            const auto result = vkCreateRenderPass(_logicalLayer->GetLogicalDevice(), &renderPassCreateInfo, nullptr, &_renderPass);

            if (result != VK_SUCCESS)
            {
                LOG_ERR(_logger, "Failed to create render pass\nResult: {}", string_VkResult(result));
                return false;
            }
            
            return true;
        }

        void Terminate()
        {
            vkDestroyRenderPass(_logicalLayer->GetLogicalDevice(), _renderPass, nullptr);
        }

        VkRenderPass GetVulkanRenderPass() const
        { return _renderPass; }

    private:
        const VulkanSwapChain* _swapChain;
        const VulkanLogicalLayer* _logicalLayer;
        VkRenderPass _renderPass;
        
        std::shared_ptr<Debug::Logger> _logger;
    };

}
    
