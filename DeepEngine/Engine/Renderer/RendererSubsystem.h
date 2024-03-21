#pragma once
#include "Core/EngineSystem.h"
#include "Vulkan/Instance/VulkanInstance.h"

#define MESSENGER_UTILS
#include "MainRenderPass.h"
#include "RendererCommandRecorder.h"
#include "TriangleRenderer.h"
#include "ImGui/ImGuiController.h"
#include "Vulkan/Semaphore.h"
#include "Vulkan/RenderPass.h"
#include "Vulkan/Debug/VulkanDebug.h"
#include "Vulkan/Fence.h"

namespace DeepEngine::Engine::Renderer
{
    
    class RendererSubsystem final : Core::EngineSubsystem
    {
    public:
        RendererSubsystem(Core::Events::EventBus& p_engineEventBus);

    protected:
        bool Init() override;

        void Destroy() override
        {
            vkDeviceWaitIdle(_vulkanInstance->GetLogicalDevice());
            
            _imGuiController->Terminate();
            delete _imGuiController;

            _mainRenderPass.reset();
            
            _factory = nullptr;
            _vulkanInstance->Terminate();
            Vulkan::VulkanDebugger::Terminate();
        }
        
        void Tick(const Core::Scene::Scene& p_scene) override
        {
            if (_isWindowMinimized)
            {
                return;
            }

            vkWaitForFences(
                _vulkanInstance->GetLogicalDevice(),
                1,
                _readyToRenderFence->GetPtr(),
                VK_TRUE,
                UINT64_MAX);

            uint32_t imageIndex;
            const auto acquireResult = vkAcquireNextImageKHR(
                _vulkanInstance->GetLogicalDevice(),
                _vulkanInstance->GetSwapchain(),
                UINT64_MAX,
                _availableImageToRenderSemaphore->GetHandler(),
                VK_NULL_HANDLE,
                &imageIndex);

            bool invalidSwapChain = false;
            if (acquireResult == VK_SUBOPTIMAL_KHR)
            {
                invalidSwapChain = true;
            }
            if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
            {
                _vulkanInstance->RecreateSwapChain();
                return;
            }
            
            vkResetFences(_vulkanInstance->GetLogicalDevice(), 1, _readyToRenderFence->GetPtr());

            _commandRecorder.RecordBuffer(
                {0.05f, 0.05f, 0.15f, 1.0f},
                imageIndex,
                _mainRenderPass,
                _renderers,
                _mainRenderPass->GetFrameImageHandler(imageIndex)
                );

            _imGuiController->Renderrr(imageIndex, p_scene);

            _commandRecorder.SubmitBuffer(_readyToRenderFence,
                { _availableImageToRenderSemaphore },
                { _finishRenderingSemaphore },
                _imGuiController->GetCommandBuffer(imageIndex));

            VkSwapchainKHR swapChains[] = { _vulkanInstance->GetSwapchain() };
            VkSemaphore waitSemaphores[] = { _finishRenderingSemaphore->GetHandler() };
            
            VkPresentInfoKHR presentInfo { };
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = waitSemaphores;
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = swapChains;
            presentInfo.pImageIndices = &imageIndex;
            presentInfo.pResults = nullptr;

            VkResult presentResult = vkQueuePresentKHR(_mainGraphicsQueue->Queue, &presentInfo);
            if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR || invalidSwapChain)
            {
                _vulkanInstance->RecreateSwapChain();
            }
            else if (presentResult != VK_SUCCESS)
            {
                VULKAN_ERR("Failed to present swapchain with returned result {}", string_VkResult(presentResult));
            }

            _imGuiController->PostRenderUpdate();
        }

    private:
        bool InitializeVulkanInstance();
        bool EnableGlfwExtensions();
        Core::Events::EventResult WindowChangedMinimizedHandler(const Core::Events::OnWindowChangeMinimized& p_event);

    private:
        Vulkan::VulkanInstance* _vulkanInstance = nullptr;
        std::shared_ptr<MainRenderPassController> _mainRenderPass = nullptr;
        Vulkan::Ref<Vulkan::Fence> _readyToRenderFence = nullptr;
        Vulkan::Ref<Vulkan::Semaphore> _availableImageToRenderSemaphore; 
        Vulkan::Ref<Vulkan::Semaphore> _finishRenderingSemaphore;

        std::unique_ptr<Vulkan::Factory> _factory;

        ImGuiController* _imGuiController;

        std::vector<TriangleRenderer> _renderers;
 
        const Vulkan::VulkanInstance::QueueInstance* _mainGraphicsQueue = nullptr;

        RendererCommandRecorder _commandRecorder;

        bool _isWindowMinimized = false;

        std::shared_ptr<Core::Events::EventListener<Core::Events::OnWindowChangeMinimized>> _wndChangeMinimizedListener;
    };
    
}
