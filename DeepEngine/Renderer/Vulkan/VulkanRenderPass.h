#pragma once
#include "Controller/BaseVulkanController.h"
#include "Instance/VulkanInstance.h"

namespace DeepEngine::Renderer::Vulkan
{
    
    class VulkanRenderPass : public BaseVulkanController
    {
    protected:
        struct RenderAttachment
        {
            uint32_t ID;
            VkAttachmentDescription* Desc;
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
        
        class RenderSubPassDescCreator
        {
            friend class VulkanRenderPass;
            RenderSubPassDescCreator(RenderSubPass& p_subPass, std::vector<VkSubpassDescription>& p_renderSubPassesDesc);

        public:
            ~RenderSubPassDescCreator();

        public:
            const RenderSubPassDescCreator& AddColorAttachment(const RenderAttachment* p_attachment, VkImageLayout p_layout) const;
            const RenderSubPassDescCreator& AddInputAttachment(const RenderAttachment* p_attachment, VkImageLayout p_layout) const;
            const RenderSubPassDescCreator& AddResolveAttachment(const RenderAttachment* p_attachment, VkImageLayout p_layout) const;
            const RenderSubPassDescCreator& SetDepthStencilAttachment(const RenderAttachment* p_attachment, VkImageLayout p_layout) const;
            const RenderSubPass* GetSubPassPtr() const;

        private:
            RenderSubPass& _subPass;
            std::vector<VkSubpassDescription>& _renderSubPassesDesc;
      };
        
    protected:
        VulkanRenderPass();
        ~VulkanRenderPass() override = default;
        
    protected:
        bool OnInitialize() final;
        void OnTerminate() final;

    public:
        VkRenderPass GetVkRenderPass() const
        { return _renderPass; }

    protected:
        void virtual Initialize() = 0;
        void virtual PostInitialize() { };
        
        void CreateRenderAttachment(const VkAttachmentDescription& p_desc, const RenderAttachment** p_attachment);
        RenderSubPassDescCreator CreateRenderSubPass(VkPipelineBindPoint p_bindPoint);

    private:
        VkRenderPass _renderPass = VK_NULL_HANDLE;
        
        std::vector<RenderAttachment> _attachments;
        std::vector<VkAttachmentDescription> _attachmentsDesc;
        std::vector<RenderSubPass> _renderSubPasses;
        std::vector<VkSubpassDescription> _renderSubPassesDesc;
    };
    
}

