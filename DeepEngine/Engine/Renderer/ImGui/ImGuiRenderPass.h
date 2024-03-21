#pragma once
#include "Engine/Renderer/Vulkan/RenderPass.h"
#include "Engine/Renderer/Vulkan/PipelineLayout.h"
#include "Engine/Renderer/Vulkan/Events/VulkanEvents.h"

namespace DeepEngine::Engine::Renderer
{

    class ImGuiRenderPassBuilder : public Vulkan::RenderPassBuilder
    {
    public:
        ImGuiRenderPassBuilder(const Vulkan::VulkanInstance* p_vulkanInstance)
            : _vulkanInstance(p_vulkanInstance)
        { }

        Vulkan::RenderSubPassHandler GetMainSubPass() const
        { return _mainSubPass; }
        
    protected:
        void DeclareAttachments() override
        {
            VkAttachmentDescription colorAttachmentDesc { };
            colorAttachmentDesc.format = _vulkanInstance->GetSwapchainCurrentFormat().format;
            colorAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
            colorAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            _colorAttachment = CreateAttachment(colorAttachmentDesc);

            _mainSubPass = CreateRenderSubPass(VK_PIPELINE_BIND_POINT_GRAPHICS);
            AttachColorToSubPass(_mainSubPass, _colorAttachment, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
            
            AttachDependencyToSubPass(
                nullptr,
                &_mainSubPass,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                0,
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
        }

    private:
        const Vulkan::VulkanInstance* _vulkanInstance;
        Vulkan::RenderAttachmentHandler _colorAttachment;
        Vulkan::RenderSubPassHandler _mainSubPass;
    };

    class ImGuiRenderPassController
    {
    public:
        ImGuiRenderPassController(const Vulkan::VulkanInstance* p_vulkanInstance)
            : _vulkanInstance(p_vulkanInstance)
        {
            ImGuiRenderPassBuilder builder(_vulkanInstance);
            _renderPass = Vulkan::Factory::SubFactory<Vulkan::RenderPass>::Create(&builder);

            _mainSubPass = builder.GetMainSubPass();
            
            _swapChainRecreatedListener = _vulkanInstance->GetVulkanEventBus().CreateListener<Vulkan::Events::OnSwapChainRecreated>();
            _swapChainRecreatedListener->BindCallback(&ImGuiRenderPassController::SwapChainRecreatedHandler, this);
            RecreateFrameBuffers();
        }

        ~ImGuiRenderPassController()
        {
            // TODO Replace with engine FrameBuffer wrapper/proxy
            for (const VkFramebuffer framebuffer : _swapChainImageFrameBuffers)
            {
                vkDestroyFramebuffer(_vulkanInstance->GetLogicalDevice(), framebuffer, nullptr);
            }
        }
        
        Vulkan::Ref<Vulkan::PipelineLayout> CreateBaseSubPassPipelineLayout() const
        {
            return Vulkan::Factory::SubFactory<Vulkan::PipelineLayout>::Create(_renderPass, _mainSubPass);
        }
        
        VkFramebuffer GetSwapChainImageFrameBufferHandler(uint32_t p_index) const
        {
            return _swapChainImageFrameBuffers[p_index];
        }

        VkRenderPass GetRenderPassHandler() const
        { return _renderPass->GetHandler(); }

    private:
        Core::Events::EventResult SwapChainRecreatedHandler(const Vulkan::Events::OnSwapChainRecreated& p_event)
        {
            RecreateFrameBuffers();
            return Core::Events::EventResult::PASS;
        }
        
        void RecreateFrameBuffers()
        {
            const std::vector<VkImageView>& imageViews = _vulkanInstance->GetSwapChainImageViews();
            glm::vec2 framebufferSize = _vulkanInstance->GetFrameBufferSize();

            _swapChainImageFrameBuffers.clear();
            _swapChainImageFrameBuffers.reserve(imageViews.size());
            
            for (uint32_t i = 0; i < imageViews.size(); i++)
            {
                VkImageView attachments[] { imageViews[i] };
                    
                VkFramebufferCreateInfo createInfo { };
                createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                createInfo.renderPass = _renderPass->GetHandler();
                createInfo.attachmentCount = 1;
                createInfo.pAttachments = attachments;
                createInfo.width = framebufferSize.x;
                createInfo.height = framebufferSize.y;
                createInfo.layers = 1;

                VkFramebuffer _framebuffer;
                VkResult result = vkCreateFramebuffer(
                    _vulkanInstance->GetLogicalDevice(),
                    &createInfo,
                    nullptr,
                    &_framebuffer);

                if (result != VK_SUCCESS)
                {
                    VULKAN_ERR("Failed to create SwapChain image view! (result: {})", string_VkResult(result));
                    continue;
                }

                _swapChainImageFrameBuffers.push_back(_framebuffer);
            }
        }
        
    private:
        const Vulkan::VulkanInstance* _vulkanInstance;
        
        std::shared_ptr<Core::Events::EventListener<Vulkan::Events::OnSwapChainRecreated>> _swapChainRecreatedListener;
        
        Vulkan::Ref<Vulkan::RenderPass> _renderPass;
        Vulkan::RenderSubPassHandler _mainSubPass;
        
        std::vector<VkFramebuffer> _swapChainImageFrameBuffers;
    };
}
