#pragma once

//#define GLFW_INCLUDE_VULKAN
#include <string>
#include <GLFW/glfw3.h>
#include "Architecture/EngineSystem.h"
#include "Architecture/EngineEvents.h"

namespace DeepEngine
{
    class WindowSubsystem : Architecture::EngineSubsystem, Architecture::EventListener<Events::OnWindowResized>
    {
    protected:
        bool EventHandler(const Events::OnWindowResized* p_event) override
        {
            INFO("Window resized {}x{}", p_event->Width, p_event->Height);
            return false;
        }
        
    public:
        
        WindowSubsystem(int p_width, int p_height, const char*);
        ~WindowSubsystem();
        
    protected:
        bool Init() override;
        void Destroy() override {}
        void Tick() override {}
        
    private:
        static void ErrorCallbackHandler(int error, const char* description);
        static void WindowResizedHandler(GLFWwindow* p_window, int p_width, int p_height);

        int _width;
        int _height;

        const char* _windowName;
        GLFWwindow* _window;
    };
}
