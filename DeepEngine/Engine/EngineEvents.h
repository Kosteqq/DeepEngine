#pragma once
#include <GLFW/glfw3.h>

#include "Core/Events/BaseEvent.h"

namespace DeepEngine::Core::Events
{

    BEGIN_GLOBAL_EVENT_DEFINITION(OnCloseRequest)
    // ...
    END_EVENT_DEFINITION

    BEGIN_GLOBAL_EVENT_DEFINITION(OnWindowResized)
    int32_t Width;
    int32_t Height;
    END_EVENT_DEFINITION

    BEGIN_GLOBAL_EVENT_DEFINITION(OnWindowFramebufferResized)
    int32_t Width;
    int32_t Height;
    END_EVENT_DEFINITION

    BEGIN_GLOBAL_EVENT_DEFINITION(OnWindowChangeMinimized)
    bool MinimizedMode;
    END_EVENT_DEFINITION

    BEGIN_GLOBAL_EVENT_DEFINITION(OnCreateGlfwContext)
    GLFWwindow* GLFWWindow;
    END_EVENT_DEFINITION

}
    
