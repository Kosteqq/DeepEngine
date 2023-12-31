#pragma once
#include "Renderer/Vulkan/RenderPass.h"
#include "Renderer/Vulkan/PipelineLayout.h"
#include "Renderer/Vulkan/Events/VulkanEvents.h"

namespace DeepEngine::Renderer
{

    class ImGuiRenderPass final : public Vulkan::RenderPass
    {
    protected:
        void Initialize() override;
        void PostInitialize() override;
        void OnTerminate() override;

    public:
        Vulkan::PipelineLayout* CreateBaseSubPassPipelineLayout();
        VkFramebuffer GetSwapchainImageVkFramebuffer(uint32_t p_index) const;

    private:
        void RecreateFrameBuffers();
        Architecture::EventResult SwapChainRecreatedHandler(const Vulkan::Events::OnSwapChainRecreated& p_event);

    private:
        std::shared_ptr<Architecture::EventListener<Vulkan::Events::OnSwapChainRecreated>> _swapChainRecreatedListener;
        
        RenderSubPass* _baseSubPass;
        const RenderAttachment* _colorAttachment;

        std::vector<VkFramebuffer> _swapchainImageFramebuffers;
    };
}
