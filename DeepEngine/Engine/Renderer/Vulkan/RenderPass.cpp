#include "RenderPass.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{

    //////////////////////////////////////////////
    //      RENDER SUBPASS DESCRIPTION CREATOR
    //////////////////////////////////////////////

    RenderPass::RenderSubPassDescCreator::RenderSubPassDescCreator(RenderSubPass& p_subPass,
        std::vector<VkSubpassDescription>& p_renderSubPassesDesc, std::vector<VkSubpassDependency>& p_dependencies)
        : _subPass(p_subPass), _renderSubPassesDesc(p_renderSubPassesDesc), _renderSubPassesDependecies(p_dependencies)
    { }

    RenderPass::RenderSubPassDescCreator::~RenderSubPassDescCreator()
    {
        _renderSubPassesDesc.emplace_back();
        VkSubpassDescription& desc = _renderSubPassesDesc.back();

        desc.colorAttachmentCount = static_cast<uint32_t>(_subPass.ColorAttachments.size());
        if (_subPass.ColorAttachments.size() > 0)
        {
            desc.pColorAttachments = _subPass.ColorAttachments.data();
        }

        desc.inputAttachmentCount = static_cast<uint32_t>(_subPass.InputAttachments.size());
        if (_subPass.InputAttachments.size() > 0)
        {
            desc.pInputAttachments = _subPass.InputAttachments.data();
        }

        // used for msaa 
        // desc.pResolveAttachments

        desc.pDepthStencilAttachment = nullptr;
        if (_subPass.UseDepthStencilAttachment)
        {
            desc.pDepthStencilAttachment = &_subPass.DepthStencilAttachment;
        }
        
        // TODO (Kostek): assign unused attachments
        // desc.preserveAttachmentCount = static_cast<uint32_t>(_subPass.PreserveAttachments.size());
        // desc.pPreserveAttachments = _subPass.PreserveAttachments.data();
    }

    const RenderPass::RenderSubPassDescCreator&
        RenderPass::RenderSubPassDescCreator::AddColorAttachment(const RenderAttachment* p_attachment, VkImageLayout p_layout) const
    {
        _subPass.ColorAttachments.push_back(
            {
                .attachment = p_attachment->ID,
                .layout = p_layout
            });
        return *this;
    }

    const RenderPass::RenderSubPassDescCreator&
        RenderPass::RenderSubPassDescCreator::AddInputAttachment(const RenderAttachment* p_attachment, VkImageLayout p_layout) const
    {
        _subPass.InputAttachments.push_back(VkAttachmentReference
            {
                .attachment = p_attachment->ID,
                .layout = p_layout
            });
        return *this;
    }

    const RenderPass::RenderSubPassDescCreator&
        RenderPass::RenderSubPassDescCreator::AddResolveAttachment(const RenderAttachment* p_attachment, VkImageLayout p_layout) const
    {
        _subPass.ResolveAttachments.push_back(VkAttachmentReference
            {
                .attachment = p_attachment->ID,
                .layout = p_layout
            });
        return *this;
    }

    const RenderPass::RenderSubPassDescCreator&
        RenderPass::RenderSubPassDescCreator::SetDepthStencilAttachment(const RenderAttachment* p_attachment, VkImageLayout p_layout) const
    {
        _subPass.UseDepthStencilAttachment = true;
        _subPass.DepthStencilAttachment = VkAttachmentReference
        {
            .attachment = p_attachment->ID,
            .layout = p_layout
        };
        return *this;
    }
    
    const RenderPass::RenderSubPassDescCreator& RenderPass::RenderSubPassDescCreator::AddDependency(const VkSubpassDependency& p_dependency) const
    {
        _renderSubPassesDependecies.push_back(p_dependency);
        return *this;
    }

    const RenderPass::RenderSubPassDescCreator& RenderPass::RenderSubPassDescCreator::GetSubPassPtr(RenderSubPass** p_subPassPtr) const
    {
        *p_subPassPtr = &_subPass;
        return *this;
    }


    //////////////////////////////////////////////
    //      VULKAN RENDER PASS
    //////////////////////////////////////////////
    
    RenderPass::RenderPass()
    {
        _attachments.reserve(16);
        _attachmentsDesc.reserve(_attachments.capacity());
        _renderSubPasses.reserve(16);
        _renderSubPassesDesc.reserve(_renderSubPasses.capacity());
        _renderSubPassesDependencies.reserve(_renderSubPasses.capacity());
    }

    bool RenderPass::OnInitialize()
    {
        Initialize();
            
        VkRenderPassCreateInfo renderPassCreateInfo { };
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(_attachmentsDesc.size());
        renderPassCreateInfo.pAttachments = _attachmentsDesc.data();
        renderPassCreateInfo.subpassCount = static_cast<uint32_t>(_renderSubPassesDesc.size());
        renderPassCreateInfo.pSubpasses = _renderSubPassesDesc.data();
        renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(_renderSubPassesDependencies.size());
        renderPassCreateInfo.pDependencies = _renderSubPassesDependencies.data();

        VULKAN_CHECK_CREATE(
            vkCreateRenderPass(GetVulkanInstanceController()->GetLogicalDevice(),
                &renderPassCreateInfo,
                nullptr,
                &_renderPass),
            "Failed to create Vulkan render pass!")

        PostInitialize();
            
        return true;
    }

    void RenderPass::OnTerminate()
    {
        vkDestroyRenderPass(GetVulkanInstanceController()->GetLogicalDevice(), _renderPass, nullptr);
    }

    void RenderPass::CreateRenderAttachment(const VkAttachmentDescription& p_desc, const RenderAttachment** p_attachment)
    {
        _attachmentsDesc.push_back(p_desc);

        _attachments.push_back(RenderAttachment {
            .ID = static_cast<uint32_t>(_attachments.size()),
            .Desc = &_attachmentsDesc.back(),
        });

        *p_attachment = &_attachments.back();
    }

    RenderPass::RenderSubPassDescCreator RenderPass::CreateRenderSubPass(VkPipelineBindPoint p_bindPoint)
    {
        _renderSubPasses.emplace_back();
        
        RenderSubPass& subPass = _renderSubPasses.back();

        subPass.ID =static_cast<uint32_t>(_renderSubPasses.size() - 1); 
        subPass.BindPoint = p_bindPoint; 
        subPass.InputAttachments.reserve(8);
        subPass.ColorAttachments.reserve(8);
        subPass.ResolveAttachments.reserve(8);
        subPass.PreserveAttachments.reserve(8);

        return RenderSubPassDescCreator { subPass, _renderSubPassesDesc, _renderSubPassesDependencies };
    }
}
