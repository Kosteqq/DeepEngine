#pragma once
#include <fstream>

#include <vulkan/vulkan.h>
#include <fmt/format.h>
#include "GLFW/glfw3.h"

#include "Debugs/Logger.h"
#include "Architecture/EngineSystem.h"
#include "Instance/VulkanInstance.h"

namespace DeepEngine::Renderer
{
    class VulkanPrototype : public Core::Architecture::EngineSubsystem
    {
        struct QueueFamilyIndices
        {
            uint32_t GraphicsFamily;
            bool FoundAny;
        };
        
    public:
        VulkanPrototype() : EngineSubsystem("Vulkan Renderer Prot")
        {
            _debugVkLogger = Core::Debug::Logger::CreateLoggerInstance("VK Validation Layer");
            _vulkanLogger = Core::Debug::Logger::CreateLoggerInstance("VULKAN");

            _vulkanInstance = VulkanInstance(_vulkanLogger);
        }

        ~VulkanPrototype()
        {
            _vulkanInstance.Terminate();
        }

    protected:
        bool Init() override
        {
            GetSupportedValidationLayers();
            for (int i = 0; i < _availableLayers.size(); i++)
            {
                if (!IsLayerAvailable(_availableLayers[i].layerName))
                {
                    FAIL_MILESTONE(InitializeValidationLayerMS);
                    return false;
                }
            }

            auto debugCreateInfo = CreateDebugInfo();
            
            if (!EnableExtensions())
            {
                ERR("Failed to enable at least one of extensions!");
                return false;
            }
            
            if (_vulkanInstance.Init({ }))
            {
                return false;
            }

            if (!InitCallbackMessenger(debugCreateInfo))
            {
                return false;
            }

            if (!CreatePhysicalDevice())
            {
                return false;
            }

            if (!InitializeLogicalDevice())
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
            if (_initialized)
            {
                // Physical device will be destroy anyway
                vkDestroyDevice(_logicalDevice, nullptr);
                // Queue also will be destroy
                DestroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);
                vkDestroyInstance(_instance, nullptr);
            }
        }

    private:
        bool EnableExtensions()
        {
            uint32_t glfwExtensionCount = 0;
            const char** glfwExtensions= glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            for (int i = 0; i < glfwExtensionCount; i++)
            {
                if (!_vulkanInstance.TryEnableExtension(glfwExtensions[i]))
                {
                    return false;
                }
            }
            
            if (!_vulkanInstance.TryEnableExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
            {
                return false;
            }

            return true;
        }

    private:
        bool IsLayerAvailable(const char* p_layerName)
        {
            for (int i = 0; i < _availableLayers.size(); i++)
            {
                if (strcmp(_availableLayers[i].layerName, p_layerName) == 0)
                {
                    return true;
                }
            }
            
            return false;
        }

        void GetSupportedValidationLayers()
        {
            uint32_t layerCount;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

            _availableLayers = std::vector<VkLayerProperties>(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount, _availableLayers.data());


            DEBUG("Found Layers:");
            for (int i = 0; i < _availableLayers.size(); i++)
            {
                DEBUG("\t{0}: {1}", _availableLayers[i].layerName, _availableLayers[i].specVersion);
            }
        }

        VkDebugUtilsMessengerCreateInfoEXT CreateDebugInfo()
        {
            VkDebugUtilsMessengerCreateInfoEXT createInfo { };
            createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            createInfo.pfnUserCallback = VulkanDebugCallback;
            createInfo.pUserData = nullptr;
            return createInfo;
        }

        bool InitCallbackMessenger(const VkDebugUtilsMessengerCreateInfoEXT& p_debugInfo)
        {
            if (CreateDebugUtilsMessengerEXT(_instance, &p_debugInfo, nullptr, &_debugMessenger) != VK_SUCCESS)
            {
                ERR("Failed to create callback messegner func");
                FAIL_MILESTONE(InitializeValidationLayerMS);
                return false;
            }

            FULFIL_MILESTONE(InitializeValidationLayerMS);
            return true;
        }

        VkResult CreateDebugUtilsMessengerEXT(VkInstance p_instance, const VkDebugUtilsMessengerCreateInfoEXT* p_createInfo,
            const VkAllocationCallbacks* p_allocator, VkDebugUtilsMessengerEXT* p_debugMessenger)
        {
            auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(p_instance, "vkCreateDebugUtilsMessengerEXT");
            if (func != nullptr)
            {
                return func(p_instance, p_createInfo, p_allocator, p_debugMessenger);
            }
            
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
        
        void DestroyDebugUtilsMessengerEXT(VkInstance p_instance, VkDebugUtilsMessengerEXT p_debugMessenger,
            const VkAllocationCallbacks* p_allocator)
        {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(p_instance, "vkDestroyDebugUtilsMessengerEXT");
            
            if (func != nullptr)
            {
                func(p_instance, p_debugMessenger, p_allocator);
            }
        }
        
        static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT p_messageSeverity,
            VkDebugUtilsMessageSeverityFlagsEXT p_messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* p_callbackData,
            void* p_usePtr)
        {
            #define CALLBACK_FORMAT "({0}) {1}"
            std::string messageType;
            switch (p_messageType)
            {
            case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
                messageType = "low-impact";
                break;
            case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
                messageType = "validation";
                break;
            case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
                messageType = "performance";
                break;
            }
            
            if (p_messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
            {
                LOG_TRACE(_debugVkLogger, CALLBACK_FORMAT, messageType, p_callbackData->pMessage);
                return VK_FALSE;
            }
            if (p_messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
            {
                LOG_DEBUG(_debugVkLogger, CALLBACK_FORMAT, messageType, p_callbackData->pMessage);
                return VK_FALSE;
            }
            if (p_messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
            {
                LOG_WARN(_debugVkLogger, CALLBACK_FORMAT, messageType, p_callbackData->pMessage);
                return VK_FALSE;
            }
            if (p_messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
            {
                LOG_ERR(_debugVkLogger, CALLBACK_FORMAT, messageType, p_callbackData->pMessage);
                return VK_FALSE;
            }
            
            return VK_FALSE;
        }
        
        static std::shared_ptr<Core::Debug::Logger> _debugVkLogger;
        VkDebugUtilsMessengerEXT _debugMessenger;

    private:
        QueueFamilyIndices GetQueueFamilies(const VkPhysicalDevice& p_device)
        {
            QueueFamilyIndices indices {0, false };

            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(p_device, &queueFamilyCount, nullptr);

            std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(p_device, &queueFamilyCount, queueFamilies.data());

            int i = 0;
            for (; i < queueFamilyCount; i++)
            {
                if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                {
                    indices.GraphicsFamily = i;
                    indices.FoundAny = true;
                    return indices;
                }
            }

            return indices;
        }

        QueueFamilyIndices _queueFamilyIndices;

    private:
        bool InitializeLogicalDevice()
        {
            VkDeviceQueueCreateInfo queueCreateInfo { };
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = _queueFamilyIndices.GraphicsFamily;
            queueCreateInfo.queueCount = 1;

            float queuePriority = 1.0f;
            queueCreateInfo.pQueuePriorities = &queuePriority;

            VkPhysicalDeviceFeatures deviceFeatures { };
            // duplicate required features

            VkDeviceCreateInfo createInfo { };
            createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            createInfo.pQueueCreateInfos = &queueCreateInfo;
            createInfo.queueCreateInfoCount = 1;
            createInfo.pEnabledFeatures = &deviceFeatures;
            createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
            createInfo.ppEnabledLayerNames = _validationLayers.data();

            if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_logicalDevice) != VK_SUCCESS)
            {
                ERR("Failed to create logical device");
                FAIL_MILESTONE(InitializeLogicalDeviceMS);
                return false;
            }

            vkGetDeviceQueue(_logicalDevice, _queueFamilyIndices.GraphicsFamily, 0, &_graphicsQueue);
            
            FULFIL_MILESTONE(InitializeLogicalDeviceMS);
            return true;
        }

        VkDevice _logicalDevice;
        VkQueue _graphicsQueue;

    private:
        std::vector<VkExtensionProperties> _availableExtensions;
        std::vector<VkLayerProperties> _availableLayers; 

        const std::vector<const char*> _validationLayers =
        {
            "VK_LAYER_KHRONOS_validation",
        };

    private:
        VulkanInstance _vulkanInstance;

        std::shared_ptr<Core::Debug::Logger> _vulkanLogger;

    private:
        DEFINE_MILESTONE(CreateVkInstanceMS);
        DEFINE_MILESTONE(InitializeValidationLayerMS);
        DEFINE_MILESTONE(InitializePhysicalDeviceMS);
        DEFINE_MILESTONE(InitializeLogicalDeviceMS);
    };
    
}
