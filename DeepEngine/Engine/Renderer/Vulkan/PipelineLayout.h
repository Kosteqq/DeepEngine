#pragma once
#include "Controller/BaseVulkanController.h"
#include "RenderPass.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{

    class PipelineLayout : public VulkanObject
    {
    public:
        PipelineLayout(VkPipelineLayout p_handler, RenderSubPassHandler p_subPass)
            : _handler(p_handler), _subPass(p_subPass)
        { }
        
        VkPipelineLayout GetHandler() const
        { return _handler; }
        
        RenderSubPassHandler GetSubPass() const
        { return _subPass; }

    private:
        const VkPipelineLayout _handler;
        const RenderSubPassHandler _subPass;
    };

    template <>
    class Factory::SubFactory<PipelineLayout>
    {
    public:
        static Ref<PipelineLayout> Create(const Ref<RenderPass>& p_renderPass, RenderSubPassHandler p_subPass);
        
    private:
        static void Terminate(VulkanObject* p_object);
    };
    
}
