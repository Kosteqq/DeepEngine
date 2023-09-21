#include "Renderer.hpp"

#include "../Debug/Debug.h"

namespace DeepEngine
{
    RendererSubsystem::RendererSubsystem()
    {
        
    }

    
    bool RendererSubsystem::Init()
    {
        _device = std::make_unique<Device>();
        if(!_device)
        {
            LOG("Failed to initialize Device");
            return false;
        }
        return true;
    }
}
