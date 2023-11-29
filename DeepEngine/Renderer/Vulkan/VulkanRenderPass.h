#pragma once
#include <unordered_map>
#include <string.h>

#include "Controller/BaseVulkanController.h"
#include "Instance/VulkanInstance.h"

#define RENDER_ATTACHMENT_DEBUG_NAME_SIZE 128
    

namespace DeepEngine::Renderer::Vulkan
{
    
    class VulkanRenderPass : public BaseVulkanController
    {
    protected:
        struct RenderAttachment
        {
            uint32_t ID;
            VkAttachmentDescription* Desc;
            char DebugName[RENDER_ATTACHMENT_DEBUG_NAME_SIZE];
        };
        
        struct RenderSubPass
        {
            uint32_t ID;
            VkPipelineBindPoint BindPoint;
            
            std::vector<VkAttachmentReference> InputAttachments;
            std::vector<VkAttachmentReference> ColorAttachments;
            std::vector<VkAttachmentReference> ResolveAttachments;
            VkAttachmentReference DepthStencilAttachment;
            std::vector<VkAttachmentReference> PreserveAttachments;
        };
        
        class RenderSubPassDescCreator
        {
            friend class VulkanRenderPass;
            RenderSubPassDescCreator(RenderSubPass& p_subPass, std::vector<VkSubpassDescription>& p_renderSubPassesDesc);

        public:
            ~RenderSubPassDescCreator();
            RenderSubPassDescCreator(const RenderSubPassDescCreator&) = delete;
            RenderSubPassDescCreator(const RenderSubPassDescCreator&&) = delete;
            RenderSubPassDescCreator& operator=(const RenderSubPassDescCreator&) = delete;

        public:
            const RenderSubPassDescCreator& AddColorAttachment(const RenderAttachment* p_attachment, VkImageLayout p_layout) const;
            const RenderSubPassDescCreator& AddInputAttachment(const RenderAttachment* p_attachment, VkImageLayout p_layout) const;
            const RenderSubPassDescCreator& AddResolveAttachment(const RenderAttachment* p_attachment, VkImageLayout p_layout) const;
            const RenderSubPassDescCreator& SetDepthStencilAttachment(const RenderAttachment* p_attachment, VkImageLayout p_layout) const;

        private:
            RenderSubPass& _subPass;
            std::vector<VkSubpassDescription>& _renderSubPassesDesc;
      };
        
    protected:
        VulkanRenderPass();

        ~VulkanRenderPass() override = default;
        
    public:
        bool OnInitialized() override;
        void OnTerminate() override;

    protected:
        void virtual Initialize() = 0;
        
        void CreateRenderAttachment(const VkAttachmentDescription& p_desc, const char* p_debugName,
            const RenderAttachment** p_attachment);
        RenderSubPassDescCreator CreateRenderSubPass(VkPipelineBindPoint p_bindPoint);

    private:
        VkRenderPass _renderPass = VK_NULL_HANDLE;
        
        std::vector<RenderAttachment> _attachments;
        std::vector<VkAttachmentDescription> _attachmentsDesc;
        std::vector<RenderSubPass> _renderSubPasses;
        std::vector<VkSubpassDescription> _renderSubPassesDesc;
    };
}

