#pragma once
#include "EventBus/BaseEvent.h"

namespace DeepEngine::Events
{

    BEGIN_GLOBAL_EVENT_DEFINITION(OnCloseRequest)
    // ...
    END_EVENT_DEFINITION

    BEGIN_GLOBAL_EVENT_DEFINITION(OnWindowResized)
    uint32_t Width;
    uint32_t Height;
    END_EVENT_DEFINITION

    BEGIN_GLOBAL_EVENT_DEFINITION(OnWindowFramebufferResized)
    uint32_t Width;
    uint32_t Height;
    END_EVENT_DEFINITION

    BEGIN_GLOBAL_EVENT_DEFINITION(OnWindowChangeMinimized)
    bool MinimizedMode;
    END_EVENT_DEFINITION

    BEGIN_GLOBAL_EVENT_DEFINITION(OnCreateGlfwContext)
    GLFWwindow* GLFWWindow;
    END_EVENT_DEFINITION

}
    
