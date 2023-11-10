#include "WindowSubsystem.hpp"
#include "Debugs/Logger.h"

namespace DeepEngine
{
    WindowSubsystem::WindowSubsystem(int p_width, int p_height, const char* p_name)
        : EngineSubsystem("Window Subsystem"), _width{p_width}, _height{p_height}, _windowName{p_name}
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

        // CHANGE THAT SHIT {
        std::string name(subsystem->_windowName);
        name += " ";
        name += std::to_string(p_width);
        name +=" ";
        name += std::to_string(p_height);
        glfwSetWindowTitle(p_window, name.c_str());
        // CHANGE THAT SHIT }
        
    }

    void WindowSubsystem::WindowFramebufferResizedHandler(GLFWwindow* p_window, int p_width, int p_height)
    {
        ENGINE_INFO("Window changed framebuffer size to: {}x{}", p_width, p_height);
        Events::OnWindowFramebufferResized event;
        glfwGetFramebufferSize(p_window, &event.Width, &event.Height);
        Architecture::PublishEvent(event);
    }

    void WindowSubsystem::WindowMinimizedHandler(GLFWwindow* p_window, int p_minimized)
    {
        ENGINE_INFO("Window changed minimized mode to: {}", p_minimized == 1);
        Events::OnWindowChangeMinimized event;
        event.MinimizedMode = p_minimized == 1;
        Architecture::PublishEvent(event);
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

        Events::OnCreateGlfwContext windowEvent;
        windowEvent.GLFWWindow = _window;
        Architecture::PublishEvent<Events::OnCreateGlfwContext>(windowEvent);
        
        INFO("Initialized with success");

        Events::OnWindowResized event;
        event.Width = 1920;
        event.Height = 1080;
        
        Architecture::PublishEvent(event);

        Events::OnWindowFramebufferResized framebufferResized;
        glfwGetFramebufferSize(_window, &framebufferResized.Width, &framebufferResized.Height);
        Architecture::PublishEvent(framebufferResized);

        return true;
    }
    
    void WindowSubsystem::Tick()
    {
        glfwPollEvents();
        if (glfwWindowShouldClose(_window))
        {
            if (!_wantToExit)
            {
                _wantToExit = true;
                Architecture::PublishEvent(Events::OnCloseRequested());
            }
        }
    }
}
