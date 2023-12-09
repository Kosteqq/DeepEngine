#pragma once
#include "Vulkan/VulkanRenderPass.h"
#include "Vulkan/VulkanPipelineLayout.h"

namespace DeepEngine::Renderer
{

    class MainRenderPass : public Vulkan::VulkanRenderPass
    {
    protected:
        void Initialize() override
        {
            VkAttachmentDescription baseColorAttachmentDesc { };
            baseColorAttachmentDesc.format = GetVulkanInstanceController()->GetSwapchainCurrentFormat().format;
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
        }

    public:
        Vulkan::VulkanPipelineLayout* CreateBaseSubPassPipelineLayout()
        {
            auto pipelineLayout = new Vulkan::VulkanPipelineLayout(this, _baseSubPass->ID);
            if (!InitializeSubController(pipelineLayout))
            {
                pipelineLayout->Terminate();
                return nullptr;
            }
            
            return pipelineLayout;
        }

    private:
        const RenderSubPass* _baseSubPass;
        const RenderAttachment* _baseColorAttachment;
    };
    
}
