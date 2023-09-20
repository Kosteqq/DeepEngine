#pragma once
#include <vulkan/vulkan.h>

#include "Debugs/Logger.h"
#include "Architecture/EngineSystem.h"

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
            uint32_t extensionCount = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);\
            INFO("Vulkan Extensions Count {0}", extensionCount);

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
