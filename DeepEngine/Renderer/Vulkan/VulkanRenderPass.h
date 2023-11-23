#pragma once
#include <unordered_map>

#include "Controller/BaseVulkanController.h"
#include "Instance/VulkanInstance.h"

namespace DeepEngine::Renderer::Vulkan
{
    class VulkanRenderPass : public BaseVulkanController
    {
    private:
        // struct Attachment
        // {
        //     const VkAttachmentDescription Desc;
        //     const uint32_t ID;
        // };
        //
        // struct RenderSubPass
        // {
        //     const uint32_t ID;
        //     // shaders
        //     // fun
        //     // hope of die
        //     // attachments
        //     // somehow GraphicsPipeline? (fuck computs)
        //     
        // };
        
    public:
        using BaseVulkanController::BaseVulkanController;
        
        
        bool OnInitialized() override
        {
            // _attachments = std::unordered_map<uint32_t, Attachment>(20); 
            // _renderSubPasses.resize(16);

            return true;
        }

        void OnTerminate() override
        { }

        void Init();

        // const Attachment& CreateColorAttachment(const VkAttachmentDescription& p_desc);
        // const RenderSubPass& CreateRenderSubPass(std::vector<RenderSubPass> p_dependencies);

    protected:
        

    private:
        // std::unordered_map<uint32_t, Attachment> _attachments;
        // std::vector<RenderSubPass> _renderSubPasses;
    };
    
}

