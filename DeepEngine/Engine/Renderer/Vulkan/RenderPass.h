#pragma once
#include "Factory.h"
#include "Controller/BaseVulkanController.h"
#include "Instance/VulkanInstance.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{
    
    struct RenderAttachmentHandler
    {
        uint32_t RenderPassID = 0;
        uint32_t AttachmentID = 0;
    };

    struct RenderSubPassHandler
    {
        uint32_t RenderPassID = 0;
        uint32_t SubPassID = 0;
    };

    class RenderPass : public VulkanObject
    {
        friend class RenderPassBuilder;

        template <VulkanObjectKind T>
        friend class Factory::SubFactory;
        
        struct RenderAttachment
        {
            RenderAttachmentHandler Handler;
        };
        
        struct RenderSubPass
        {
            uint32_t ID;
            VkPipelineBindPoint BindPoint;
            
            std::vector<VkAttachmentReference> InputAttachments;
            std::vector<VkAttachmentReference> ColorAttachments;
            std::vector<VkAttachmentReference> ResolveAttachments;
            bool UseDepthStencilAttachment;
            VkAttachmentReference DepthStencilAttachment;
            std::vector<VkAttachmentReference> PreserveAttachments;
        };

        RenderPass(VkRenderPass p_handler, const std::vector<RenderAttachmentHandler>& p_attachments)
            : _handler(p_handler), _attachments(p_attachments)
        {
            
        }

    public:
        VkRenderPass GetHandler() const
        { return _handler; }

        std::vector<RenderAttachmentHandler> GetAttachments() const
        { return _attachments; }

    private:
        const VkRenderPass _handler;
        const std::vector<RenderAttachmentHandler> _attachments;
    };

    class RenderPassBuilder
    {
        template <VulkanObjectKind T>
        friend class Factory::SubFactory;
        
    protected:
        struct AttachmentDescription 
        {
            RenderAttachmentHandler Handler;
            VkAttachmentDescription Desc;
        };
        
        struct SubPassDescription
        {
            RenderSubPassHandler Handler;
            VkPipelineBindPoint BindPoint;
            
            std::vector<VkAttachmentReference> InputAttachments;
            std::vector<VkAttachmentReference> ColorAttachments;
            std::vector<VkAttachmentReference> ResolveAttachments;
            bool UseDepthStencilAttachment;
            VkAttachmentReference DepthStencilAttachment;
            std::vector<VkAttachmentReference> PreserveAttachments;
        };

        RenderPassBuilder();

    public:
        virtual ~RenderPassBuilder() = default;

    protected:
        virtual void DeclareAttachments() = 0;

        RenderAttachmentHandler CreateAttachment(const VkAttachmentDescription& p_desc);
        RenderSubPassHandler CreateRenderSubPass(VkPipelineBindPoint p_bindPoint);

        void AttachColorToSubPass(RenderSubPassHandler p_subPass, RenderAttachmentHandler p_attachment, VkImageLayout p_layout);
        void AttachInputToSubPass(RenderSubPassHandler p_subPass, RenderAttachmentHandler p_attachment, VkImageLayout p_layout);
        void AttachResolveToSubPass(RenderSubPassHandler p_subPass, RenderAttachmentHandler p_attachment, VkImageLayout p_layout);
        void AttachDepthStencilToSubPass(RenderSubPassHandler p_subPass, RenderAttachmentHandler p_attachment, VkImageLayout p_layout);
        void AttachDependencyToSubPass(RenderSubPassHandler* p_srcSubPass, RenderSubPassHandler* p_dstSubPass,
                                       VkPipelineStageFlags p_srcStageMask, VkPipelineStageFlags p_dstStageMask,
                                       VkAccessFlags p_srcAccessFlags, VkAccessFlags p_dstAccessFlags);

    private:
        uint32_t _currentRenderPassID = 0;
        std::vector<AttachmentDescription> _attachments;
        std::vector<SubPassDescription> _subpasses;
        std::vector<VkSubpassDependency> _dependecies;
    };
    
    template<>
    class Factory::SubFactory<RenderPass>
    {
    public:
        static Ref<RenderPass> Create(RenderPassBuilder* p_builder, Ref<VulkanObject> p_parent = nullptr);

    private:
        static void Terminate(VulkanObject* p_object);

        static uint32_t _idCounter;
    };
    
}

