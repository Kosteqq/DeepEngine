#include "WindowSubsystem.hpp"
#include "Debug/Logger.h"

namespace DeepEngine
{
    WindowSubsystem::WindowSubsystem(Core::Events::EventBus& p_engineEventBus, int p_width, int p_height, const char* p_name)
        : EngineSubsystem(p_engineEventBus, "Window Subsystem"), _width{p_width}, _height{p_height}, _windowName{p_name}
    {
        
    }

    WindowSubsystem::~WindowSubsystem()
    {
        glfwDestroyWindow(_window);
        glfwTerminate();
    }

    void WindowSubsystem::ErrorCallbackHandler(int error, const char* description)
    {
        fprintf(stderr, "Error: %s\n", description);
    }

    void WindowSubsystem::WindowResizedHandler(GLFWwindow* p_window, int p_width, int p_height)
    {
        auto subsystem = (WindowSubsystem*)glfwGetWindowUserPointer(p_window);
        subsystem->_width = p_width;
        subsystem->_height = p_height;
        
        glfwSetWindowTitle(p_window, fmt::format("{0} {1}x{2}", subsystem->_windowName, p_width, p_height).c_str());
    }

    void WindowSubsystem::WindowFramebufferResizedHandler(GLFWwindow* p_window, int p_width, int p_height)
    {
        ENGINE_INFO("Window changed framebuffer size to: {}x{}", p_width, p_height);
        Core::Events::OnWindowFramebufferResized event;
        glfwGetFramebufferSize(p_window, &event.Width, &event.Height);
        
        auto subsystem = (WindowSubsystem*)glfwGetWindowUserPointer(p_window);
        subsystem->_internalSubsystemEventBus.Publish(event);
    }

    void WindowSubsystem::WindowMinimizedHandler(GLFWwindow* p_window, int p_minimized)
    {
        ENGINE_INFO("Window changed minimized mode to: {}", p_minimized == 1);
        Core::Events::OnWindowChangeMinimized event;
        event.MinimizedMode = p_minimized == 1;
        
        auto subsystem = (WindowSubsystem*)glfwGetWindowUserPointer(p_window);
        subsystem->_internalSubsystemEventBus.Publish(event);
    }

    bool WindowSubsystem::Init()
    {
        glfwSetErrorCallback(ErrorCallbackHandler);
        if (!glfwInit())
        {
            ERR("Failed to initilize library");
            return false;
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwSwapInterval(1);
        // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        
        _window = glfwCreateWindow(_width, _height, _windowName, nullptr, nullptr);
        if (_window == nullptr)
        {
            ERR("Failed to create GLFW Window");
            return false;
        }
        glfwSetWindowSizeCallback(_window, WindowResizedHandler);
        glfwSetFramebufferSizeCallback(_window, WindowFramebufferResizedHandler);
        glfwSetWindowIconifyCallback(_window, WindowMinimizedHandler);
        glfwSetWindowUserPointer(_window, this);

        Core::Events::OnCreateGlfwContext windowEvent;
        windowEvent.GLFWWindow = _window;
        _internalSubsystemEventBus.Publish(windowEvent);
        
        INFO("Initialized with success");

        Core::Events::OnWindowResized event;
        glfwGetWindowSize(_window, &event.Width, &event.Height);
        _internalSubsystemEventBus.Publish(event);
        
        Core::Events::OnWindowFramebufferResized framebufferResizedEvent;
        glfwGetFramebufferSize(_window, &framebufferResizedEvent.Width, &framebufferResizedEvent.Height);
        _internalSubsystemEventBus.Publish(framebufferResizedEvent);

        return true;
    }
    
    void WindowSubsystem::Tick(const Core::Scene::Scene& p_scene)
    {
        glfwPollEvents();
        if (glfwWindowShouldClose(_window))
        {
            if (!_wantToExit)
            {
                _wantToExit = true;
                _internalSubsystemEventBus.Publish<Core::Events::OnCloseRequest>();
            }
        }
    }
}
