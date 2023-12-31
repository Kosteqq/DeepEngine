#pragma once
#include "Architecture/EngineSystem.h"
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

namespace DeepEngine::Renderer
{
    struct FrameInFlight
    {
        Vulkan::CommandBuffer* _commandBuffer;
        Vulkan::Fence* _fence;
        
    };
    
    class RendererSubsystem final : Architecture::EngineSubsystem
    {
    public:
        RendererSubsystem(Architecture::EventBus& p_engineEventBus)
            : EngineSubsystem(p_engineEventBus, "Renderer")
        { 
            _vulkanInstance = new Vulkan::VulkanInstance(p_engineEventBus, _internalSubsystemEventBus);
            _wndChangeMinimizedListener = _internalSubsystemEventBus.CreateListener<EngineEvents::OnWindowChangeMinimized>();
            _wndChangeMinimizedListener->BindCallback(&RendererSubsystem::WindowChangedMinimizedHandler, this);
        }

    protected:
        bool Init() override;

        void Destroy() override
        {
            vkDeviceWaitIdle(_vulkanInstance->GetLogicalDevice());
            
            _imGuiController->Terminate();
            delete _imGuiController;
            
            _commandRecorder.Terminate();
            _vulkanInstance->Terminate();
            Vulkan::VulkanDebugger::Terminate();
        }
        
        void Tick() override
        {
            if (_isWindowMinimized)
            {
                return;
            }

            vkWaitForFences(
                _vulkanInstance->GetLogicalDevice(),
                1,
                _readyToRenderFence->GetVkFencePtr(),
                VK_TRUE,
                UINT64_MAX);

            uint32_t imageIndex;
            const auto acquireResult = vkAcquireNextImageKHR(
                _vulkanInstance->GetLogicalDevice(),
                _vulkanInstance->GetSwapchain(),
                UINT64_MAX,
                _availableImageToRenderSemaphore->GetVkSemaphore(),
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
            
            vkResetFences(_vulkanInstance->GetLogicalDevice(), 1, _readyToRenderFence->GetVkFencePtr());

            _commandRecorder.RecordBuffer(
                {0.05f, 0.05f, 0.15f, 1.0f},
                imageIndex,
                _mainRenderPass,
                _renderers,
                _mainRenderPass->GetVkImage(imageIndex)
                );

            _imGuiController->Renderrr(imageIndex);

            _commandRecorder.SubmitBuffer(_readyToRenderFence,
                { _availableImageToRenderSemaphore },
                { _finishRenderingSemaphore },
                _imGuiController->GetCommandBuffer(imageIndex));

            VkSwapchainKHR swapChains[] = { _vulkanInstance->GetSwapchain() };
            VkSemaphore waitSemaphores[] = { _finishRenderingSemaphore->GetVkSemaphore() };
            
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
        Architecture::EventResult WindowChangedMinimizedHandler(const EngineEvents::OnWindowChangeMinimized& p_event);

    private:
        Vulkan::VulkanInstance* _vulkanInstance = nullptr;
        MainRenderPass* _mainRenderPass = nullptr;
        Vulkan::Fence* _readyToRenderFence = nullptr;
        Vulkan::Semaphore* _availableImageToRenderSemaphore = nullptr; 
        Vulkan::Semaphore* _finishRenderingSemaphore = nullptr;

        ImGuiController* _imGuiController;

        std::vector<TriangleRenderer> _renderers;
 
        const Vulkan::VulkanInstance::QueueInstance* _mainGraphicsQueue = nullptr;

        RendererCommandRecorder _commandRecorder;

        bool _isWindowMinimized = false;

        std::shared_ptr<Architecture::EventListener<EngineEvents::OnWindowChangeMinimized>> _wndChangeMinimizedListener;
    };
    
}
