#pragma once
#include <fstream>

#define NOMINMAX
#include <vulkan/vulkan.h>
#include <fmt/format.h>

#include "Debugs/Logger.h"
#include "Architecture/EngineSystem.h"
#include "Instance/VulkanDebug.h"
#include "Instance/VulkanInstance.h"
#include "Instance/VulkanLogicalLayer.h"
#include "Instance/VulkanPhysicalLayer.h"
#include "Instance/VulkanRenderPass.h"
#include "Instance\VulkanPipelineLayout.h"
#include "Instance/VulkanSwapChain.h"
#include "Window/WindowSubsystem.hpp"

namespace DeepEngine::Renderer
{
    class VulkanPrototype : public Architecture::EngineSubsystem, Architecture::EventListener<Events::OnCreateGlfwContext>
    {
    public:
        VulkanPrototype() : EngineSubsystem("Vulkan Renderer Prot")
        {
            _vulkanLogger = Debug::Logger::CreateLoggerInstance("VULKAN");
        }

        ~VulkanPrototype()
        {
            _pipeline->Terminate();
            _renderPass->Terminate();
            _pipeline->Terminate();
            _swapChain->Terminate();
            // Queue also will be destroy
            _logicalLayer->Terminate();
            // Physical device will be destroy anyway

            vkDestroySurfaceKHR(_vulkanInstance->GetInstance(), _surface, nullptr);
            _vulkanInstance->Terminate();
            _vulkanDebug->Terminate();

            delete _pipeline;
            delete _renderPass;
            delete _swapChain;
            delete _logicalLayer;
            delete _physicalLayer;
            delete _vulkanInstance;
            delete _vulkanDebug;
        }

    protected:
        bool EventHandler(const Events::OnCreateGlfwContext* p_event) override
        {
            _glfwWindow = p_event->GLFWWindow;
            return false;
        }

    protected:
        bool Init() override
        {
            const auto messageTypes = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            
            // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
            const auto messageLevels = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
                
            _vulkanDebug = new VulkanDebug(_vulkanLogger, messageLevels, messageTypes);

            if (!_vulkanDebug->TryAddLayer("VK_LAYER_KHRONOS_validation"))
            {
                WARN("Failed to enable base vulkan validation layer");
            }
            
            _vulkanInstance = new VulkanInstance(_vulkanLogger);
            
            if (!EnableExtensions())
            {
                ERR("Failed to enable at least one of extensions!");
                return false;
            }

            if (!_vulkanInstance->Init(_vulkanDebug))
            {
                FAIL_MILESTONE(InitializeVulkanDebug);
            }
            else
            {
                FULFIL_MILESTONE(InitializeVulkanDebug);
            }
            
            if (!_vulkanDebug->Init(&_vulkanInstance->GetInstance()))
            {
                FAIL_MILESTONE(CreateVulkanInstance);
                return false;
            }
            FULFIL_MILESTONE(CreateVulkanInstance);

            // DIRTY
            if (_glfwWindow == nullptr)
            {
                ERR("GLFW context is null but should be already created!");
            }
            
            if (glfwCreateWindowSurface(_vulkanInstance->GetInstance(), _glfwWindow, nullptr, &_surface))
            {
                return false;
            }
            
            
            // \DIRTY
            

            _physicalLayer = new VulkanPhysicalLayer(_vulkanLogger, _vulkanInstance);

            if (!_physicalLayer->Init())
            {
                FAIL_MILESTONE(InitializeVulkanPhysicalDevice);
                return false;
            }
            FULFIL_MILESTONE(InitializeVulkanPhysicalDevice);
            
            if (!_physicalLayer->TryEnableExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME))
            {
                return false;
            }

            _logicalLayer = new VulkanLogicalLayer(_vulkanLogger, _physicalLayer, _surface);
            // For now only single queue is supported!
            _logicalLayer->AddQueue(0, VK_QUEUE_GRAPHICS_BIT, true);

            if (!_logicalLayer->Init(_vulkanDebug))
            {
                FAIL_MILESTONE(InitializeVulkanLogicalDevice);
                return false;
            }
            FULFIL_MILESTONE(InitializeVulkanLogicalDevice);

            uint32_t width = 0;
            uint32_t height = 0;
            // _subsystemsManager->GetSubsystem<WindowSubsystem>()->GetFramebufferSize(&width, &height);
            _swapChain = new VulkanSwapChain(_vulkanLogger, _physicalLayer, _logicalLayer, _surface, width, height);
            
            if (!_swapChain->Init())
            {
                return false;
            }

            _renderPass = new VulkanRenderPass(_vulkanLogger, _logicalLayer, _swapChain);

            if (!_renderPass->Initialize())
            {
                return false;
            }

            _pipeline = new VulkanPipeline(_vulkanLogger, _logicalLayer, _swapChain, _renderPass);

            if (!_pipeline->Initialize())
            {
                return false;
            }

            return true;
        }
        
        void Tick() override
        {
            
        }

        void Destroy() override
        {
        }
        
    private:
        bool EnableExtensions()
        {
            uint32_t glfwExtensionCount = 0;
            const char** glfwExtensions= glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            for (int i = 0; i < glfwExtensionCount; i++)
            {
                if (!_vulkanInstance->TryEnableExtension(glfwExtensions[i]))
                {
                    return false;
                }
            }
            
            if (!_vulkanInstance->TryEnableExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
            {
                return false;
            }

            return true;
        }

    private:
        VkSurfaceKHR _surface;
        GLFWwindow* _glfwWindow;

    private:
        VulkanDebug* _vulkanDebug;
        VulkanInstance* _vulkanInstance;
        VulkanPhysicalLayer* _physicalLayer;
        VulkanLogicalLayer* _logicalLayer;
        VulkanSwapChain* _swapChain;
        VulkanRenderPass* _renderPass;
        VulkanPipeline* _pipeline;
        
        std::shared_ptr<Debug::Logger> _vulkanLogger;

    private:
        DEFINE_MILESTONE(CreateVulkanInstance);
        DEFINE_MILESTONE(InitializeVulkanDebug);
        DEFINE_MILESTONE(InitializeVulkanPhysicalDevice);
        DEFINE_MILESTONE(InitializeVulkanLogicalDevice);
    };
    
}
