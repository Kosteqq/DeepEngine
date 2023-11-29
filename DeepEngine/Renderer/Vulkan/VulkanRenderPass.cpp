#include "VulkanRenderPass.h"

namespace DeepEngine::Renderer::Vulkan
{

    //////////////////////////////////////////////
    //      RENDER SUBPASS DESCRIPTION CREATOR
    //////////////////////////////////////////////

    VulkanRenderPass::RenderSubPassDescCreator::RenderSubPassDescCreator(RenderSubPass& p_subPass, std::vector<VkSubpassDescription>& p_renderSubPassesDesc)
        : _subPass(p_subPass), _renderSubPassesDesc(p_renderSubPassesDesc)
    { }

    VulkanRenderPass::RenderSubPassDescCreator::~RenderSubPassDescCreator()
    {
        VkSubpassDescription desc { };
        _renderSubPassesDesc.push_back(desc);

        desc.colorAttachmentCount = static_cast<uint32_t>(_subPass.ColorAttachments.size());
        desc.pColorAttachments = _subPass.ColorAttachments.data();

        desc.inputAttachmentCount = static_cast<uint32_t>(_subPass.InputAttachments.size());
        desc.pInputAttachments = _subPass.InputAttachments.data();

        // used for msaa 
        // desc.pResolveAttachments

        desc.pDepthStencilAttachment = &_subPass.DepthStencilAttachment;

        // TODO (Kostek): assign unused attachments
        // desc.preserveAttachmentCount = static_cast<uint32_t>(_subPass.PreserveAttachments.size());
        // desc.pPreserveAttachments = _subPass.PreserveAttachments.data();
    }

    const VulkanRenderPass::RenderSubPassDescCreator&
        VulkanRenderPass::RenderSubPassDescCreator::AddColorAttachment(const RenderAttachment* p_attachment, VkImageLayout p_layout) const
    {
        _subPass.ColorAttachments.push_back(VkAttachmentReference
            {
                .attachment = p_attachment->ID,
                .layout = p_layout
            });
        return *this;
    }

    const VulkanRenderPass::RenderSubPassDescCreator&
        VulkanRenderPass::RenderSubPassDescCreator::AddInputAttachment(const RenderAttachment* p_attachment, VkImageLayout p_layout) const
    {
        _subPass.InputAttachments.push_back(VkAttachmentReference
            {
                .attachment = p_attachment->ID,
                .layout = p_layout
            });
        return *this;
    }

    const VulkanRenderPass::RenderSubPassDescCreator&
        VulkanRenderPass::RenderSubPassDescCreator::AddResolveAttachment(const RenderAttachment* p_attachment, VkImageLayout p_layout) const
    {
        _subPass.ResolveAttachments.push_back(VkAttachmentReference
            {
                .attachment = p_attachment->ID,
                .layout = p_layout
            });
        return *this;
    }

    const VulkanRenderPass::RenderSubPassDescCreator&
        VulkanRenderPass::RenderSubPassDescCreator::SetDepthStencilAttachment(const RenderAttachment* p_attachment, VkImageLayout p_layout) const
    {
        _subPass.DepthStencilAttachment = VkAttachmentReference
        {
            .attachment = p_attachment->ID,
            .layout = p_layout
        };
        return *this;
    }



    //////////////////////////////////////////////
    //      VULKAN RENDER PASS
    //////////////////////////////////////////////
    
    VulkanRenderPass::VulkanRenderPass()
    {
        _attachments.reserve(16);
        _attachmentsDesc.reserve(_attachments.capacity());
        _renderSubPasses.reserve(16);
        _renderSubPassesDesc.reserve(_renderSubPasses.capacity());
    }

    bool VulkanRenderPass::OnInitialized()
    {
        Initialize();
            
        VkRenderPassCreateInfo renderPassCreateInfo { };
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(_attachmentsDesc.size());
        renderPassCreateInfo.pAttachments = _attachmentsDesc.data();
        renderPassCreateInfo.subpassCount = static_cast<uint32_t>(_renderSubPassesDesc.size());
        renderPassCreateInfo.pSubpasses = _renderSubPassesDesc.data();
        renderPassCreateInfo.dependencyCount = 0;
        renderPassCreateInfo.pDependencies = nullptr;

        VULKAN_CHECK_CREATE(
            vkCreateRenderPass(GetVulkanInstanceController()->GetLogicalDevice(),
                &renderPassCreateInfo,
                nullptr,
                &_renderPass),
            "Failed to create Vulkan render pass!")
            
        return true;
    }

    void VulkanRenderPass::OnTerminate()
    { }

    void VulkanRenderPass::CreateRenderAttachment(const VkAttachmentDescription& p_desc, const char* p_debugName,
        const RenderAttachment** p_attachment)
    {
        _attachmentsDesc.push_back(p_desc);
            
        RenderAttachment attachment {
            .ID = static_cast<uint32_t>(_attachments.size()),
            .Desc = &_attachmentsDesc.back(),
        };

        strcpy_s(attachment.DebugName, RENDER_ATTACHMENT_DEBUG_NAME_SIZE, p_debugName);

        _attachments.push_back(attachment);

        *p_attachment = &_attachments.back();
    }

    VulkanRenderPass::RenderSubPassDescCreator VulkanRenderPass::CreateRenderSubPass(VkPipelineBindPoint p_bindPoint)
    {
        RenderSubPass subPass {
            .ID = static_cast<uint32_t>(_renderSubPasses.size()),
            .BindPoint = p_bindPoint,
        };
                
        subPass.InputAttachments.reserve(8);
        subPass.ColorAttachments.reserve(8);
        subPass.ResolveAttachments.reserve(8);
        subPass.PreserveAttachments.reserve(8);

        _renderSubPasses.push_back(subPass);

        return RenderSubPassDescCreator { subPass, _renderSubPassesDesc };
    }
}
