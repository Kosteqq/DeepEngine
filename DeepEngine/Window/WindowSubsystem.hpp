#pragma once
#define NOMINMAX

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "Architecture/EngineSystem.h"
#include "Architecture/EngineEvents.h"

namespace DeepEngine
{
    class WindowSubsystem : Architecture::EngineSubsystem
    {
    public:
        WindowSubsystem(Architecture::EventBus& p_engineEventBus, int p_width, int p_height, const char*);
        ~WindowSubsystem();

        GLFWwindow* GetGlfwWindow() const
        { return _window; }

        void GetFramebufferSize(uint32_t* p_width, uint32_t* p_height) const
        {
            return glfwGetFramebufferSize(_window, reinterpret_cast<int*>(p_width), reinterpret_cast<int*>(p_height));
        }

        bool WantsToExit()
        { return _wantToExit; }
        
    protected:
        bool Init() override;
        void Destroy() override {}
        void Tick(const Architecture::Scene::Scene& p_scene) override;

    private:
        static void ErrorCallbackHandler(int error, const char* description);
        static void WindowResizedHandler(GLFWwindow* p_window, int p_width, int p_height);
        static void WindowFramebufferResizedHandler(GLFWwindow* p_window, int p_width, int p_height);
        static void WindowMinimizedHandler(GLFWwindow* p_window, int p_minimized);

        int _width;
        int _height;

        bool _wantToExit = false;

        const char* _windowName;
        GLFWwindow* _window;
    };
}
