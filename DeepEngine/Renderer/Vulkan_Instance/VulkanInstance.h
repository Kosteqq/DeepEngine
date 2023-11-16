#pragma once
#include "VulkanPCH.h"

#include <glm/glm.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Architecture/EngineEvents.h"
#include "Architecture/Events.h"
#include "Renderer/Vulkan_Debug/VulkanDebug.h"


namespace DeepEngine::Renderer::Vulkan
{

    class VulkanInstance :
        public Architecture::EventListener<Events::OnCreateGlfwContext>,
        public Architecture::EventListener<Events::OnWindowFramebufferResized>,
        public Architecture::EventListener<Events::OnWindowChangeMinimized>
    {
    public:
        struct QueueInstance
        {
            const VkQueue Queue;
            const VkQueueFlagBits Bits;
            const VkQueueFamilyProperties Family;
            const bool SupportsSurface;
            const uint32_t ID;
        };
        
    public:
        VulkanInstance()
        {
            PreinitializeInstance();
            PreinitializePhysicalDevice();

            _enabledInstanceExtensionNames.reserve(32);
            _enabledPhysicalExtensionNames.reserve(32);
            _queuesCreateInfo.reserve(16);
        }
        
        ~VulkanInstance()
        { Terminate(); }

        void Terminate()
        {
            TerminateInstance();
            TerminatePhysicalDevice();
            TerminateLogicalDevice();
            TerminateSwapChain();
        }

    public:
        bool InitializeInstance();
        bool InitializePhysicalDevice();
        bool InitializeLogicalDevice();
        bool InitializeSwapChain() { return true; }
        
    private:
        void PreinitializeInstance(); 
        void PreinitializePhysicalDevice();
        void PreinitializeLogicalDevice();
        
        void TerminateInstance();
        void TerminatePhysicalDevice();
        void TerminateLogicalDevice();
        void TerminateSwapChain() { }

    public:
        VkPhysicalDevice GetPhysicalDevice() const
        { return _physicalDevice; }
        
        VkDevice GetLogicalDevice() const
        { return _logicalDevice; }

        glm::vec2  GetFrameBufferSize() const
        { return _frameBufferSize; }

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

        void SetSwapchainFormat(VkFormat p_imageFormat);

    private:
        bool FindMatchingPhysicalDevice(const std::vector<VkPhysicalDevice>& p_devices);

    protected:
        bool EventHandler(const Events::OnCreateGlfwContext* p_event) override { return false; }
        bool EventHandler(const Events::OnWindowFramebufferResized* p_event) override { return false; }
        bool EventHandler(const Events::OnWindowChangeMinimized* p_event) override { return false; }

    private:
        GLFWwindow* _glfwWindow;
        VkInstance _instance;
        
        VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties _physicalDeviceProperties;
        VkPhysicalDeviceFeatures _physicalDeviceFeatures;
        VkDevice _logicalDevice = VK_NULL_HANDLE;
        std::vector<QueueInstance> _queues;
        VkSwapchainKHR _swapchain = VK_NULL_HANDLE;
    
        glm::vec2 _frameBufferSize;
        VkSurfaceKHR _surface = VK_NULL_HANDLE;


        std::vector<VkExtensionProperties> _availableInstanceExtensions;
        std::vector<const char*> _enabledInstanceExtensionNames;
        
        std::vector<const char*> _enabledPhysicalExtensionNames;
        
        float _queuePriority = 1;
        std::vector<VkQueueFamilyProperties> _availableQueueFamilies;
        std::vector<VkDeviceQueueCreateInfo> _queuesCreateInfo;
        
        std::vector<VkImage> _swapChainImages;
        std::vector<VkImageView> _swapChainImageViews;

        VkFormat _swapchainImageFormat;
        VkSurfaceFormatKHR _swapchainFormat;
        VkPresentModeKHR _swapchainPresent;
        VkExtent2D _swapchainExtent;
    };
    
}
