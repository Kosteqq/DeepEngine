#pragma once
#include "Debugs/Logger.h"

#include <vulkan/vulkan.h>

namespace DeepEngine::Renderer
{
    class VulkanLogicalLayer
    {
        VulkanLogicalLayer(const VkInstance& p_instance, std::shared_ptr<Core::Debug::Logger> p_logger);
        
    private:
        const VkInstance& _instance;
    };
}
