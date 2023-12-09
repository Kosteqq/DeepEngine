#pragma once
#include "Architecture/EngineSystem.h"
#include "Vulkan/Instance/VulkanInstance.h"

#define MESSENGER_UTILS
#include "MainRenderPass.h"
#include "Vulkan/VulkanRenderPass.h"
#include "Vulkan/VulkanShaderModule.h"
#include "Vulkan/Debug/VulkanDebug.h"
#include "Vulkan/VulkanFence.h"

namespace DeepEngine::Renderer
{
    
    class RendererSubsystem final : Architecture::EngineSubsystem,
                                    Architecture::EventListener<Events::OnWindowChangeMinimized>
    {
    public:
        RendererSubsystem() : EngineSubsystem("Renderer")
        { 
            _vulkanInstance = new Vulkan::VulkanInstance();
        }

    protected:
        bool Init() override
        {
            if (!InitializeVulkanInstance())
            {
                return false;
            }

            _acquireImageFence = new Vulkan::VulkanFence();
            if (!_vulkanInstance->InitializeSubController(_acquireImageFence))
            {
                return false;
            }

            _mainRenderPass = new MainRenderPass();
            if (!_vulkanInstance->InitializeSubController(_mainRenderPass))
            {
                return false;
            }

            return true;
        }
        
        void Destroy() override
        {
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
                &_acquireImageFence->GetFence(),
                VK_TRUE,
                UINT64_MAX);
            
            vkResetFences(_vulkanInstance->GetLogicalDevice(), 1, &_acquireImageFence->GetFence());

            uint32_t imageIndex;
            const auto acquireResult = vkAcquireNextImageKHR(
                _vulkanInstance->GetLogicalDevice(),
                _vulkanInstance->GetSwapchain(),
                UINT64_MAX,
                VK_NULL_HANDLE,
                _acquireImageFence->GetFence(),
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

            VkSwapchainKHR swapChains[] = { _vulkanInstance->GetSwapchain() };
            
            VkPresentInfoKHR presentInfo { };
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = 0;
            presentInfo.pWaitSemaphores = nullptr;
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
        }

    protected:
        bool EventHandler(const Events::OnWindowChangeMinimized* p_event) override
        {
            _isWindowMinimized = p_event->MinimizedMode;
            return false;
        }

    private:
        bool InitializeVulkanInstance();

        bool EnableGlfwExtensions();

    private:
        Vulkan::VulkanInstance* _vulkanInstance = nullptr;
        Vulkan::VulkanFence* _acquireImageFence = nullptr;
        MainRenderPass* _mainRenderPass = nullptr;
        
        const Vulkan::VulkanInstance::QueueInstance* _mainGraphicsQueue = nullptr;

        bool _isWindowMinimized = false;
    };
    
}
