#pragma once
#include "Vulkan/RenderPass.h"
#include "Vulkan/PipelineLayout.h"
#include "Vulkan/Events/VulkanEvents.h"

namespace DeepEngine::Renderer
{

    class MainRenderPass : public Vulkan::RenderPass
    {
    protected:
        void Initialize() override
        {
            auto vulkanController = GetVulkanInstanceController();
            
            VkAttachmentDescription baseColorAttachmentDesc { };
            baseColorAttachmentDesc.format = vulkanController->GetSwapchainCurrentFormat().format;
            baseColorAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
            baseColorAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            baseColorAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            baseColorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            baseColorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            baseColorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            baseColorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            
            CreateRenderAttachment(baseColorAttachmentDesc, &_baseColorAttachment);
            
            _baseSubPass = CreateRenderSubPass(VK_PIPELINE_BIND_POINT_GRAPHICS)
                .AddColorAttachment(_baseColorAttachment, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                .GetSubPassPtr();

            _swapChainRecreatedListener = vulkanController->GetVulkanEventBus().CreateListener<Vulkan::Events::OnSwapChainRecreated>();
            _swapChainRecreatedListener->BindCallback(&MainRenderPass::SwapChainRecreatedHandler, this);
        }

        void PostInitialize() override
        {
            RecreateFrameBuffers();
        }

    public:
        Vulkan::PipelineLayout* CreateBaseSubPassPipelineLayout()
        {
            auto pipelineLayout = new Vulkan::PipelineLayout(this, _baseSubPass->ID);
            if (!InitializeSubController(pipelineLayout))
            {
                pipelineLayout->Terminate();
                return nullptr;
            }
            
            return pipelineLayout;
        }

        VkFramebuffer GetSwapchainImageVkFramebuffer(uint32_t p_index)
        {
            return _swapchainImageFramebuffers[p_index];
        }

    private:
        void RecreateFrameBuffers()
        {
            const std::vector<VkImageView>& imageViews = GetVulkanInstanceController()->GetSwapChainImageViews();
            glm::vec2 framebufferSize = GetVulkanInstanceController()->GetFrameBufferSize();

            _swapchainImageFramebuffers.clear();
            _swapchainImageFramebuffers.reserve(imageViews.size());
            
            for (uint32_t i = 0; i < imageViews.size(); i++)
            {
                VkImageView attachments[] { imageViews[i] };
                    
                VkFramebufferCreateInfo createInfo { };
                createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                createInfo.renderPass = GetVkRenderPass();
                createInfo.attachmentCount = 1;
                createInfo.pAttachments = attachments;
                createInfo.width = framebufferSize.x;
                createInfo.height = framebufferSize.y;
                createInfo.layers = 1;

                VkFramebuffer _framebuffer;
                VkResult result = vkCreateFramebuffer(
                    GetVulkanInstanceController()->GetLogicalDevice(),
                    &createInfo,
                    nullptr,
                    &_framebuffer);

                if (result != VK_SUCCESS)
                {
                    VULKAN_ERR("Failed to create SwapChain image view! (result: {})", string_VkResult(result));
                    continue;
                }

                _swapchainImageFramebuffers.push_back(_framebuffer);
            }
            
        }

        Architecture::EventResult SwapChainRecreatedHandler(const Vulkan::Events::OnSwapChainRecreated& p_event)
        {
            RecreateFrameBuffers();
            return Architecture::Internal::PASS;
        }

    private:
        std::shared_ptr<Architecture::EventListener<Vulkan::Events::OnSwapChainRecreated>> _swapChainRecreatedListener;
        
        const RenderSubPass* _baseSubPass;
        const RenderAttachment* _baseColorAttachment;

        std::vector<VkFramebuffer> _swapchainImageFramebuffers;
    };
    
}
