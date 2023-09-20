#pragma once
#include <fstream>
#include <vulkan/vulkan.h>

#include "Debugs/Logger.h"
#include "Architecture/EngineSystem.h"
#include "GLFW/glfw3.h"

namespace DeepEngine::Renderer
{

    class VulkanPipeline
    {
    public:
        VulkanPipeline()
        {
            CreateGraphicsPipeline();
        }

    private:
        static std::vector<char> ReadFile(const char* p_filepath)
        {
            std::ifstream file { p_filepath, std::ios::ate | std::ios::binary  };

            if (!file.is_open())
            {
                ENGINE_ERR("Failed to open \"{0}\"", p_filepath);
                return std::vector<char>();
            }

            size_t fileSize = static_cast<size_t>(file.tellg());
            std::vector<char> buffer(fileSize);

            file.seekg(0);
            file.read(buffer.data(), fileSize);
            file.close();
            return buffer;
        }
        void CreateGraphicsPipeline()
        {
            ReadFile("simple_shader.vert.spv");
            ReadFile("simple_shader.frag.spv");
        }
    };

    class VulkanPrototype : public Core::Architecture::EngineSubsystem
    {
    public:
        VulkanPrototype() : EngineSubsystem("Vulkan Renderer Prot")
        {
        }

    protected:
        bool Init() override
        {
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
            
        }

    private:
        bool CreateVulkanInstance()
        {
            VkApplicationInfo appInfo { };
            appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.pNext = nullptr; // Point to extensions info
            appInfo.pApplicationName = "Deep Engine Editor";
            appInfo.applicationVersion = 0;
            appInfo.pEngineName = "Deep Engine";
            appInfo.engineVersion = 0;
            appInfo.apiVersion = VK_API_VERSION_1_3;

            uint32_t glfwExtensionCount = 0;
            const char** glfwExtensions;
            glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
            
            VkInstanceCreateInfo instanceCreateInfo { };
            instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            instanceCreateInfo.pApplicationInfo = &appInfo;
            instanceCreateInfo.enabledExtensionCount = glfwExtensionCount;
            instanceCreateInfo.ppEnabledExtensionNames = glfwExtensions;
            instanceCreateInfo.enabledLayerCount = 0;

            if (vkCreateInstance(&instanceCreateInfo, nullptr, &_instance) != VK_SUCCESS)
            {
                FAIL_MILESTONE(CreateVkInstanceMS);
                ENGINE_ERR("Failed to create instance");
                return false;
            }

            return true;
        }

    private:
        VkInstance _instance;

    private:
        DEFINE_MILESTONE(CreateVkInstanceMS);
    };
    
}
