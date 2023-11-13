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
            PreinitilizeInstance();
        }
        
        ~VulkanInstance()
        { Terminate(); }

        void Terminate()
        {
            InitializeInstance();
            InitializePhysicalDevice();
            InitializeLogicalDevice();
            InitializeSwapChain();
        }

    public:
        bool InitializeInstance();
        bool InitializePhysicalDevice();
        bool InitializeLogicalDevice() { return true; }
        bool InitializeSwapChain() { return true; }
        
    private:
        void PreinitilizeInstance(); 
        
        void TerminateInstance();
        void TerminatePhysicalDevice();
        void TerminateLogicalDevice();
        void TerminateSwapChain();

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
        
        inline bool IsPhysicalExtensionAvailable(const VkExtensionProperties& p_extension) const;
        bool IsPhysicalExtensionAvailable(const char* p_extensionName) const;

        inline void EnablePhysicalExtension(const VkExtensionProperties& p_extension);
        void EnablePhysicalExtension(const char* p_extensionName);

        void SetSwapchainFormat(VkFormat p_imageFormat);

    private:
        template<typename Argument, typename Func>
        bool IsNameInCollection(const char* p_name, const std::vector<Argument>& p_collection, const Func& p_getName) const
        {
            for (uint32_t i = 0; i < p_collection.size(); i++)
            {
                if (strcmp(p_name, p_getName(p_collection[i])) == 0)
                {
                    return true;
                }
            }
            return false;
        }

    protected:
        bool EventHandler(const Events::OnCreateGlfwContext* p_event) override { return false; }
        bool EventHandler(const Events::OnWindowFramebufferResized* p_event) override { return false; }
        bool EventHandler(const Events::OnWindowChangeMinimized* p_event) override { return false; }

    private:
        GLFWwindow* _glfwWindow;
        VkInstance _instance;
        VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
        VkDevice _logicalDevice = VK_NULL_HANDLE;
        std::vector<QueueInstance> _queues;
        VkSwapchainKHR _swapchain = VK_NULL_HANDLE;
    
        glm::vec2 _frameBufferSize;
        VkSurfaceKHR _surface = VK_NULL_HANDLE;


        std::vector<VkExtensionProperties> _availableInstanceExtensions;
        std::vector<const char*> _enabledInstanceExtensionNames;
        
        std::vector<VkExtensionProperties> _availablePhysicalExtensions;
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
