#pragma once
#include <fstream>
#include <vulkan/vulkan.h>

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

            if (!CreateVulkanInstance())
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
                vkDestroyInstance(_instance, nullptr);
            }
        }

    private:
        bool CreateVulkanInstance()
        {
            GetExtensions();

            if (!CheckGlfwExtensionsMatch())
            {
                FAIL_MILESTONE(CreateVkInstanceMS);
                ENGINE_ERR("GLFW requred extensions all not available");
                return false;
            }

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
            instanceCreateInfo.enabledLayerCount = 0;

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
                if (!IsExtensionsAvailable(glfwExtensions[i]))
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
                    return;
                }
            }

            _enabledExtensions.push_back(p_extensionName);
        }

        bool IsExtensionsAvailable(const char* p_extensionName)
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
        std::vector<VkExtensionProperties> _availableExtensions;
        std::vector<const char*> _enabledExtensions;
        VkInstance _instance;

        bool _initialized;

    private:
        DEFINE_MILESTONE(CreateVkInstanceMS);
    };
    
}
