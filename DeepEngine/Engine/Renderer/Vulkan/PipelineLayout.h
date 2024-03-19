#pragma once
#include "Controller/BaseVulkanController.h"
#include "RenderPass.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{

    class RenderPass;

    class PipelineLayout : public BaseVulkanController
    {
    public:
        PipelineLayout(RenderPass* p_renderPass, uint32_t p_subPassIndex);
        ~PipelineLayout() override = default;

        RenderPass* GetRenderPass() const
        { return _renderPass; }

        uint32_t GetSubPassIndex() const
        { return _subPassIndex; }

        VkPipelineLayout GetVkPipelineLayout() const
        { return _pipelineLayout; }

    protected:
        bool OnInitialize() final;
        void OnTerminate() final;

    protected:
        VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
        const uint32_t _subPassIndex;
        RenderPass* _renderPass;
    };

    class PipelineLayout2 : public VulkanObject
    {
    public:
        PipelineLayout2(VkPipelineLayout p_handler, RenderSubPassHandler p_subPass)
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
    class Factory::SubFactory<PipelineLayout2>
    {
    public:
        static Ref<PipelineLayout2> Create(Ref<RenderPass2> p_renderPass, RenderSubPassHandler p_subPass)
        {
            VkPipelineLayoutCreateInfo createInfo { };
            createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            createInfo.setLayoutCount = 0;
            createInfo.pSetLayouts = nullptr;
            createInfo.pushConstantRangeCount = 0;
            createInfo.pPushConstantRanges = nullptr;

            VkPipelineLayout handler;
            
            vkCreatePipelineLayout(
                _bindFactory->_vulkanInstance.GetLogicalDevice(),
                &createInfo,
                nullptr,
                &handler);

            auto pipelineLayout = new PipelineLayout2(handler, p_subPass);
            
            return CreateObject(pipelineLayout, Terminate, p_renderPass);
        }

        static void Terminate(VulkanObject* p_object)
        {
            vkDestroyPipelineLayout(
                _bindFactory->_vulkanInstance.GetLogicalDevice(),
                ((PipelineLayout2*)p_object)->GetHandler(),
                nullptr);
        }
        
    };
    
}
