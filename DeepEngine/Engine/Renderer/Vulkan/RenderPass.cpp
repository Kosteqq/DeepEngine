#include "RenderPass.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{

    uint32_t Factory::SubFactory<RenderPass>::_idCounter = 0;

    RenderPassBuilder::RenderPassBuilder()
    {
        _attachments.reserve(16);
        _subpasses.reserve(16);
        _dependecies.reserve(16);
    }

    RenderAttachmentHandler RenderPassBuilder::CreateAttachment(const VkAttachmentDescription& p_desc)
    {
        const uint32_t id = static_cast<uint32_t>(_attachments.size());
        const RenderAttachmentHandler handler { _currentRenderPassID, id };
            
        _attachments.push_back({ handler, p_desc });
            
        return handler;
    }

    RenderSubPassHandler RenderPassBuilder::CreateRenderSubPass(VkPipelineBindPoint p_bindPoint)
    {
        const uint32_t id = static_cast<uint32_t>(_subpasses.size());
        const RenderSubPassHandler handler { _currentRenderPassID, id };

        SubPassDescription subPass { handler, p_bindPoint };
        subPass.ColorAttachments.reserve(16);
        subPass.InputAttachments.reserve(16);
        subPass.ResolveAttachments.reserve(16);
        subPass.PreserveAttachments.reserve(16);
            
        _subpasses.push_back(subPass);
        
        return handler;
    }

    void RenderPassBuilder::AttachColorToSubPass(RenderSubPassHandler p_subPass, RenderAttachmentHandler p_attachment,
        VkImageLayout p_layout)
    {
        _subpasses[p_subPass.SubPassID].ColorAttachments.emplace_back(
            p_attachment.AttachmentID,
            p_layout
        );
    }

    void RenderPassBuilder::AttachInputToSubPass(RenderSubPassHandler p_subPass, RenderAttachmentHandler p_attachment,
        VkImageLayout p_layout)
    {
        _subpasses[p_subPass.SubPassID].InputAttachments.emplace_back(
            p_attachment.AttachmentID,
            p_layout
        );
    }

    void RenderPassBuilder::AttachResolveToSubPass(RenderSubPassHandler p_subPass, RenderAttachmentHandler p_attachment,
        VkImageLayout p_layout)
    {
        _subpasses[p_subPass.SubPassID].ResolveAttachments.emplace_back(
            p_attachment.AttachmentID,
            p_layout
        );
    }

    void RenderPassBuilder::AttachDepthStencilToSubPass(RenderSubPassHandler p_subPass,
        RenderAttachmentHandler p_attachment, VkImageLayout p_layout)
    {
        _subpasses[p_subPass.SubPassID].UseDepthStencilAttachment = true;
            
        _subpasses[p_subPass.SubPassID].DepthStencilAttachment = {
            p_attachment.AttachmentID,
            p_layout
        };
    }

    void RenderPassBuilder::AttachDependencyToSubPass(RenderSubPassHandler* p_srcSubPass,
        RenderSubPassHandler* p_dstSubPass, VkPipelineStageFlags p_srcStageMask, VkPipelineStageFlags p_dstStageMask,
        VkAccessFlags p_srcAccessFlags, VkAccessFlags p_dstAccessFlags)
    {
        // TODO: Setup access flags & stages in subpass

        _dependecies.emplace_back(
            p_srcSubPass != nullptr ? p_srcAccessFlags : VK_SUBPASS_EXTERNAL,
            p_dstSubPass->SubPassID,
            p_srcStageMask,
            p_dstStageMask,
            p_srcAccessFlags,
            p_dstAccessFlags);
    }

    Ref<RenderPass> Factory::SubFactory<RenderPass>::Create(RenderPassBuilder* p_builder, Ref<VulkanObject> p_parent)
    {
        p_builder->DeclareAttachments();

        _idCounter++;

        const std::vector<RenderPassBuilder::AttachmentDescription>& attachments = p_builder->_attachments;

        auto attachmentsDesc = std::make_unique<VkAttachmentDescription[]>(attachments.size());
        std::vector<RenderAttachmentHandler> attachmentsHandlers(attachments.size());
        for (uint32_t i = 0; i < attachments.size(); i++)
        {
            attachmentsDesc[i] = attachments[i].Desc;
            attachmentsHandlers[i] = attachments[i].Handler;
        }
            
        const std::vector<RenderPassBuilder::SubPassDescription>& subpasses = p_builder->_subpasses;
        auto subpassesDesc = std::make_unique<VkSubpassDescription[]>(subpasses.size());
        for (uint32_t i = 0; i < subpasses.size(); i++)
        {
            auto& subpass = subpasses[i];
                
            subpassesDesc[i] = VkSubpassDescription { };

            subpassesDesc[i].colorAttachmentCount = (uint32_t)subpass.ColorAttachments.size();
            if (!subpass.ColorAttachments.empty())
            {
                subpassesDesc[i].pColorAttachments = subpass.ColorAttachments.data();
            }

            subpassesDesc[i].inputAttachmentCount = (uint32_t)subpass.InputAttachments.size();
            if (!subpass.InputAttachments.empty())
            {
                subpassesDesc[i].pInputAttachments = subpass.InputAttachments.data();
            }

            // TODO (Kostek): assign resolve attachments
            // subpassesDesc[i].pResolveAttachments = subpass.ResolveAttachments.data();

            subpassesDesc[i].pDepthStencilAttachment = nullptr;
                
            if (subpass.UseDepthStencilAttachment)
            {
                subpassesDesc[i].pDepthStencilAttachment = &subpass.DepthStencilAttachment;
            }

            // TODO (Kostek): assign preserve attachments
            // desc.preserveAttachmentCount = static_cast<uint32_t>(_subPass.PreserveAttachments.size());
            // desc.pPreserveAttachments = _subPass.PreserveAttachments.data();
        }
            
        VkRenderPassCreateInfo createInfo { };
        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        createInfo.pAttachments = attachmentsDesc.get();
        createInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
        createInfo.pSubpasses = subpassesDesc.get();
        createInfo.dependencyCount = static_cast<uint32_t>(p_builder->_dependecies.size());
        createInfo.pDependencies = p_builder->_dependecies.data();

        VkRenderPass vulkanHandler;

        VULKAN_ASSERT_RESULT_V(
            vkCreateRenderPass(_bindFactory->_vulkanInstance.GetLogicalDevice(), &createInfo, nullptr, &vulkanHandler),
            nullptr)

        return CreateObject(new RenderPass(vulkanHandler, attachmentsHandlers), Terminate, p_parent);
    }

    void Factory::SubFactory<RenderPass>::Terminate(VulkanObject* p_object)
    {
        vkDestroyRenderPass(
            _bindFactory->_vulkanInstance.GetLogicalDevice(),
            ((RenderPass*)p_object)->GetHandler(),
            nullptr);
    }
}
