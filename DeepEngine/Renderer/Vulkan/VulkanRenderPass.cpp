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
        _subPass.ColorAttachments.push_back(
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

    const VulkanRenderPass::RenderSubPass* VulkanRenderPass::RenderSubPassDescCreator::GetSubPassPtr() const
    {
        return &_subPass;
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

    bool VulkanRenderPass::OnInitialize()
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
    {
        vkDestroyRenderPass(GetVulkanInstanceController()->GetLogicalDevice(), _renderPass, nullptr);
    }

    void VulkanRenderPass::CreateRenderAttachment(const VkAttachmentDescription& p_desc, const RenderAttachment** p_attachment)
    {
        _attachmentsDesc.push_back(p_desc);

        _attachments.push_back(RenderAttachment {
            .ID = static_cast<uint32_t>(_attachments.size()),
            .Desc = &_attachmentsDesc.back(),
        });

        *p_attachment = &_attachments.back();
    }

    VulkanRenderPass::RenderSubPassDescCreator VulkanRenderPass::CreateRenderSubPass(VkPipelineBindPoint p_bindPoint)
    {
        _renderSubPasses.emplace_back();
        
        RenderSubPass& subPass = _renderSubPasses.back();

        subPass.ID =static_cast<uint32_t>(_renderSubPasses.size()); 
        subPass.BindPoint = p_bindPoint; 
        subPass.InputAttachments.reserve(8);
        subPass.ColorAttachments.reserve(8);
        subPass.ResolveAttachments.reserve(8);
        subPass.PreserveAttachments.reserve(8);

        return RenderSubPassDescCreator { subPass, _renderSubPassesDesc };
    }
}
