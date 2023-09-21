#pragma once
#include <fstream>
#include <vulkan/vulkan.h>

#include <fmt/format.h>

#include "Debugs/Logger.h"
#include "Architecture/EngineSystem.h"
#include "GLFW/glfw3.h"

namespace DeepEngine::Renderer
{
    class VulkanPrototype : public Core::Architecture::EngineSubsystem
    {
    public:
        VulkanPrototype() : EngineSubsystem("Vulkan Renderer Prot")
        {
        }

    protected:
        bool Init() override
        {
            _initialized = false;
             uint32_t extensionCount = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);\
            INFO("Vulkan Extensions Count {0}", extensionCount);

            GetSupportedValidationLayers();
            for (int i = 0; i < _availableLayers.size(); i++)
            {
                if (!IsLayerAvailable(_availableLayers[i].layerName))
                {
                    FAIL_MILESTONE(InitializeValidationLayerMS);
                    return false;
                }
            }

            FULFIL_MILESTONE(InitializeValidationLayerMS);

            auto debugCreateInfo = CreateDebugInfo();
            
            if (!CreateVulkanInstance(debugCreateInfo))
            {
                return false;
            }

            if (!InitCallbackMessenger(debugCreateInfo))
            {
                return false;
            }

            FULFIL_MILESTONE(CreateVkInstanceMS);
            return true;
        }
        
        void Tick() override
        {
            
        }

        void Destroy() override
        {
            if (_initialized)
            {
                DestroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);
                vkDestroyInstance(_instance, nullptr);
            }
        }

    private:
        bool CreateVulkanInstance(const VkDebugUtilsMessengerCreateInfoEXT& p_debugInfo)
        {
            GetExtensions();

            if (!CheckGlfwExtensionsMatch())
            {
                FAIL_MILESTONE(CreateVkInstanceMS);
                ENGINE_ERR("GLFW requred extensions all not available");
                return false;
            }

            if (!IsExtensionAvailable(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
            {
                FAIL_MILESTONE(CreateVkInstanceMS);
                ENGINE_ERR("Extension VK_EXT_DEBUG_UTILS_EXTENSION_NAME is not supported!");
                return false;
            }
            EnableExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

            VkApplicationInfo appInfo { };
            appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.pNext = nullptr; // Point to extensions info
            appInfo.pApplicationName = "Deep Engine Editor";
            appInfo.applicationVersion = 0;
            appInfo.pEngineName = "Deep Engine";
            appInfo.engineVersion = 0;
            appInfo.apiVersion = VK_API_VERSION_1_3;
            
            auto enabledExtensionNames = GetEnabledExtensionNames();

            VkInstanceCreateInfo instanceCreateInfo { };
            instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            instanceCreateInfo.pApplicationInfo = &appInfo;
            instanceCreateInfo.enabledExtensionCount = _enabledExtensions.size();
            instanceCreateInfo.ppEnabledExtensionNames = enabledExtensionNames.get();
            // Replace to disable debug
            // instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
            // instanceCreateInfo.ppEnabledLayerNames = _validationLayers.data();
            instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
            instanceCreateInfo.ppEnabledLayerNames = _validationLayers.data();
            instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&p_debugInfo;

            if (vkCreateInstance(&instanceCreateInfo, nullptr, &_instance) != VK_SUCCESS)
            {
                FAIL_MILESTONE(CreateVkInstanceMS);
                ENGINE_ERR("Failed to create instance");
                return false;
            }

            _initialized = true;
            return true;
        }

        void GetExtensions()
        {
            uint32_t extensionsCount = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);
            
            _availableExtensions = std::vector<VkExtensionProperties> (extensionsCount);
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, _availableExtensions.data());

            DEBUG("Found Extension:");
            for (int i = 0; i < _availableExtensions.size(); i++)
            {
                DEBUG("\t{0}: {1}", _availableExtensions[i].extensionName, _availableExtensions[i].specVersion);
            }
        }

        std::unique_ptr<const char*> GetEnabledExtensionNames()
        {
            const char** names = new const char*[_enabledExtensions.size()];
            std::unique_ptr<const char*> namesPtr { names };

            for (int i = 0; i < _enabledExtensions.size(); i++)
            {
                names[i] = _enabledExtensions[i];
            }

            return std::move(namesPtr);
        }

        bool CheckGlfwExtensionsMatch()
        {
            uint32_t glfwExtensionCount = 0;
            const char** glfwExtensions;
            glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            for (int i = 0; i < glfwExtensionCount; i++)
            {
                if (!IsExtensionAvailable(glfwExtensions[i]))
                {
                    return false;
                }
                
                EnableExtension(glfwExtensions[i]);
            }

            return true;
        }

        void EnableExtension(const VkExtensionProperties& p_extension)
        {
            EnableExtension(p_extension.extensionName);
        }

        void EnableExtension(const char* p_extensionName)
        {
            for (int i = 0; i < _availableExtensions.size(); i++)
            {
                if (strcmp(_availableExtensions[i].extensionName, p_extensionName) == 0)
                {
                    _enabledExtensions.push_back(p_extensionName);
                    return;
                }
            }
        }

        bool IsExtensionAvailable(const char* p_extensionName)
        {
            for (int i = 0; i < _availableExtensions.size(); i++)
            {
                if (strcmp(_availableExtensions[i].extensionName, p_extensionName) == 0)
                {
                    return true;
                }
            }
            
            return false;
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
                ENGINE_TRACE(CALLBACK_FORMAT, messageType, p_callbackData->pMessage);
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
        std::vector<VkExtensionProperties> _availableExtensions;
        std::vector<VkLayerProperties> _availableLayers; 
        std::vector<const char*> _enabledExtensions;
        VkInstance _instance;

        bool _initialized;

        const std::vector<const char*> _validationLayers =
        {
            "VK_LAYER_KHRONOS_validation",
        };

    private:
        DEFINE_MILESTONE(InitializeValidationLayerMS);
        DEFINE_MILESTONE(CreateVkInstanceMS);
    };
    
}
