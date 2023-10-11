#pragma once
#include "Events.h"

namespace DeepEngine::Events
{

    DECLARE_EVENT(OnCloseRequested)
    {
        
    };

    DECLARE_EVENT(OnWindowResized)
    {
        uint32_t Width;
        uint32_t Height;
    };

    DECLARE_EVENT(OnCreateGlfwContext)
    {
        GLFWwindow* GLFWWindow;
    };

}
    
