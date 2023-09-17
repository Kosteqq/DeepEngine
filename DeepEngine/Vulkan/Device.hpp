#pragma once

#include <vulkan/vulkan_core.h>

#include "../Architecture/EngineSystem.h"

namespace DeepEngine
{
    class Device 
    {
    public:
        Device();
        ~Device();

    protected:
      

    private:
        void initVulkan();
        void CreateInstance();

        VkInstance _instance;
        
    };
}
