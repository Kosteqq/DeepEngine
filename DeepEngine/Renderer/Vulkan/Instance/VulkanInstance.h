#pragma once
#include "../VulkanPCH.h"
#include <concepts>

#include <glm/glm.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Architecture/EngineEvents.h"
#include "Architecture/Events.h"
#include "Renderer/Vulkan/Debug/VulkanDebug.h"
#include "../Controller/BaseVulkanController.h"

namespace DeepEngine::Renderer::Vulkan
{

    class VulkanInstance :
        public BaseVulkanController,
        public Architecture::EventListener<Events::OnCreateGlfwContext>,
        public Architecture::EventListener<Events::OnWindowFramebufferResized>,
        public Architecture::EventListener<Events::OnWindowChangeMinimized>
    {
    public:
        struct QueueInstance
        {
            VkQueue Queue;
            const VkQueueFlagBits Bits;
            const std::vector<VkQueueFamilyProperties> Family;
            const bool SupportsSurface;
            const uint32_t ID;
        };
        
    public:
        VulkanInstance()
        { 
            _enabledInstanceExtensionNames.reserve(32);
            _enabledPhysicalExtensionNames.reserve(32);
            _queuesCreateInfo.reserve(16);
            _queueInstances.reserve(16);

            _vulkanInstance = this;

            PreinitializeInstance();
        }

    private:
        bool OnInitialized() override
        { return true; }

        void OnTerminate() override
        {
            TerminateSwapChain();
            TerminateLogicalDevice();
            TerminateInstance();
        }

    public:
        bool InitializeInstance()
        {
            return OnInitializeInstance(); 
        }
        
        bool InitializePhysicalDevice()
        {
            if (OnInitializePhysicalDevice()
                && OnInitializeSurface())
            {
                PreinitializeLogicalDevice();
                return true;
            }
            return false;
        }
        
        bool InitializeLogicalDevice()
        {
            return OnInitializeLogicalDevice();
        }
        
        bool InitializeSwapChain()
        {
            return OnInitializeSwapChain();
        }

        bool TryAddQueueToCreate(VkQueueFlagBits p_requiredFeatures, bool p_needSurfaceSupport,
            const QueueInstance** p_outputInstance);
        
    private:
        bool OnInitializeInstance();
        bool OnInitializePhysicalDevice();
        bool OnInitializeSurface();
        bool OnInitializeLogicalDevice();
        bool OnInitializeSwapChain();
        
        void PreinitializeInstance(); 
        void PreinitializeLogicalDevice();
        
        void TerminateInstance();
        void TerminateLogicalDevice();
        void TerminateSwapChain();

    public:
        VkPhysicalDevice GetPhysicalDevice() const
        { return _physicalDevice; }
        
        VkDevice GetLogicalDevice() const
        { return _logicalDevice; }
        
        const std::vector<VkSurfaceFormatKHR>& GetAvailableSurfaceFormats() const 
        { return _availableSurfaceFormats; }
        
        const std::vector<VkPresentModeKHR>& GetAvailableSurfacePresentModes() const 
        { return _availableSurfacePresentModes; } 

        glm::vec2  GetFrameBufferSize() const
        { return _swapChainCurrentFrameBufferSize; }

        VkSwapchainKHR GetSwapchain() const
        { return _swapchain; }
        
        VkSurfaceKHR  GetSurface() const
        { return _surface; }

    public:
        inline bool IsInstanceExtensionAvailable(const VkExtensionProperties& p_extension) const;
        bool IsInstanceExtensionAvailable(const char* p_extensionName) const;

        inline void EnableInstanceExtension(const VkExtensionProperties& p_extension);
        void EnableInstanceExtension(const char* p_extensionName);

        inline void EnablePhysicalExtension(const VkExtensionProperties& p_extension);
        void EnablePhysicalExtension(const char* p_extensionName);

        bool IsSurfaceFormatAvailable() const;
        bool IsSurfacePresentModeAvailable() const;

        void SetSwapChainFormat(VkSurfaceFormatKHR p_format, VkPresentModeKHR p_presentMode, bool p_ignoreRecreate = false);
        void RecreateSwapChain();

    private:
        bool FindMatchingPhysicalDevice(const std::vector<VkPhysicalDevice>& p_devices);

    protected:
        bool EventHandler(const Events::OnCreateGlfwContext* p_event) override
        {
            _glfwWindow = p_event->GLFWWindow;
            return false;
        }
        
        bool EventHandler(const Events::OnWindowFramebufferResized* p_event) override
        {
            _swapChainCurrentFrameBufferSize = { p_event->Width, p_event->Height };
            _isSwapChainValid = false;
            return false;
        }
        bool EventHandler(const Events::OnWindowChangeMinimized* p_event) override { return false; }

    private:
        GLFWwindow* _glfwWindow;
        VkInstance _instance;
        
        VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties _physicalDeviceProperties;
        VkPhysicalDeviceFeatures _physicalDeviceFeatures;
        VkDevice _logicalDevice = VK_NULL_HANDLE;
        std::vector<QueueInstance> _queues;

        VkSurfaceCapabilitiesKHR _availableSurfaceCapabilities;
        std::vector<VkSurfaceFormatKHR> _availableSurfaceFormats;
        std::vector<VkPresentModeKHR> _availableSurfacePresentModes;
        VkSurfaceKHR _surface = VK_NULL_HANDLE;

        VkSurfaceFormatKHR _swapchainCurrentFormat;
        VkPresentModeKHR _swapChainCurrentPresentMode;
        glm::vec2 _swapChainCurrentFrameBufferSize;

        VkSwapchainKHR _swapchain = VK_NULL_HANDLE;
        bool _isSwapChainValid = false;
        std::vector<VkImage> _swapChainImages;
        std::vector<VkImageView> _swapChainImageViews;

        std::vector<VkExtensionProperties> _availableInstanceExtensions;
        std::vector<const char*> _enabledInstanceExtensionNames;
        
        std::vector<const char*> _enabledPhysicalExtensionNames;
        
        float _queuePriority = 1;
        std::vector<VkQueueFamilyProperties> _availableQueueFamilies;
        std::vector<VkDeviceQueueCreateInfo> _queuesCreateInfo;
        std::vector<QueueInstance> _queueInstances;
    };
    
}
