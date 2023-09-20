#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Architecture/EngineSystem.h"

namespace DeepEngine
{
    class WindowSubsystem : public Core::Architecture::EngineSubsystem
    {
        
    public:
        
        WindowSubsystem(int p_width, int p_height, const char*);
        ~WindowSubsystem();
        
    protected:
        bool Init() override;
        void Destroy() override {}
        void Tick() override;
        
    private:
        
        
        static void ErrorCallbackHandler(int error, const char* description);
        static void WindowResizedHandler(GLFWwindow* p_window, int p_width, int p_height);

        int _width;
        int _height;

        const char* _windowName;
        GLFWwindow* _window;
    };
}
