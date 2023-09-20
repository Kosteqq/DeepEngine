#pragma once
#include <fstream>
#include <vulkan/vulkan.h>

#include "Debugs/Logger.h"
#include "Architecture/EngineSystem.h"

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
            VulkanPipeline();

            return false;
        }
        
        void Tick() override
        {
            
        }

        void Destroy() override
        {
            
        }
    };
    
}
