#pragma once
#include "Engine/Renderer/Vulkan/RenderPass.h"
#include "Engine/Renderer/Vulkan/PipelineLayout.h"
#include "Engine/Renderer/Vulkan/Events/VulkanEvents.h"

namespace DeepEngine::Engine::Renderer
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
        Core::Events::EventResult SwapChainRecreatedHandler(const Vulkan::Events::OnSwapChainRecreated& p_event);

    private:
        std::shared_ptr<Core::Events::EventListener<Vulkan::Events::OnSwapChainRecreated>> _swapChainRecreatedListener;
        
        RenderSubPass* _baseSubPass;
        const RenderAttachment* _colorAttachment;

        std::vector<VkFramebuffer> _swapchainImageFramebuffers;
    };
}
