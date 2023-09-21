#include "Renderer.hpp"

#include "Debugs/Logger.h"

namespace DeepEngine
{
    RendererSubsystem::RendererSubsystem():EngineSubsystem("Renderer")
    {
        
    }

    
    bool RendererSubsystem::Init()
    {
        _device = std::make_unique<Device>();
        if(!_device)
        {
            ERR("Failed to initialize Device");
            return false;
        }
        return true;
    }
}
