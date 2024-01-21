#pragma once
#include "Controller/BaseVulkanController.h"
#include "Instance/VulkanInstance.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{
    
    class RenderPass : public BaseVulkanController
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
            friend class RenderPass;
            RenderSubPassDescCreator(RenderSubPass& p_subPass, std::vector<VkSubpassDescription>& p_renderSubPassesDesc,
                std::vector<VkSubpassDependency>& p_dependencies);

        public:
            ~RenderSubPassDescCreator();

            const RenderSubPassDescCreator& AddColorAttachment(const RenderAttachment* p_attachment, VkImageLayout p_layout) const;
            const RenderSubPassDescCreator& AddInputAttachment(const RenderAttachment* p_attachment, VkImageLayout p_layout) const;
            const RenderSubPassDescCreator& AddResolveAttachment(const RenderAttachment* p_attachment, VkImageLayout p_layout) const;
            const RenderSubPassDescCreator& SetDepthStencilAttachment(const RenderAttachment* p_attachment, VkImageLayout p_layout) const;
            const RenderSubPassDescCreator& AddDependency(const VkSubpassDependency& p_dependency) const;
            const RenderSubPassDescCreator& GetSubPassPtr(RenderSubPass** p_subPassPtr) const;

        private:
            RenderSubPass& _subPass;
            std::vector<VkSubpassDescription>& _renderSubPassesDesc;
            std::vector<VkSubpassDependency>& _renderSubPassesDependecies;
      };
        
    protected:
        RenderPass();
        ~RenderPass() override = default;
        
    protected:
        bool OnInitialize() final;
        void OnTerminate() override;

    public:
        VkRenderPass GetVkRenderPass() const
        { return _renderPass; }

    protected:
        void virtual Initialize() = 0;
        void virtual PostInitialize() { }
        
        void CreateRenderAttachment(const VkAttachmentDescription& p_desc, const RenderAttachment** p_attachment);
        RenderSubPassDescCreator CreateRenderSubPass(VkPipelineBindPoint p_bindPoint);

    private:
        VkRenderPass _renderPass = VK_NULL_HANDLE;
        
        std::vector<RenderAttachment> _attachments;
        std::vector<VkAttachmentDescription> _attachmentsDesc;
        std::vector<RenderSubPass> _renderSubPasses;
        std::vector<VkSubpassDescription> _renderSubPassesDesc;
        std::vector<VkSubpassDependency> _renderSubPassesDependencies;
    };
    
}

