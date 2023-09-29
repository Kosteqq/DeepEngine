#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "Architecture/EngineSystem.h"

namespace DeepEngine
{
    class WindowSubsystem : public Core::Architecture::EngineSubsystem
    {
    public:
        static bool WantsToExit;
        
    public:
        WindowSubsystem(int p_width, int p_height, const char*);
        ~WindowSubsystem();

        GLFWwindow* GetGlfwWindow() const
        { return _window; }

        void GetFramebufferSize(uint32_t* p_width, uint32_t* p_height) const
        {
            return glfwGetFramebufferSize(_window, reinterpret_cast<int*>(p_width), reinterpret_cast<int*>(p_height));
        }
        
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
