#include "WindowSubsystem.hpp"
#include "Debug/Logger.h"

namespace DeepEngine
{
    WindowSubsystem::WindowSubsystem(int p_width, int p_height, const char* p_name) : _width{p_width}, _height{p_height}, _windowName{p_name}
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

    bool WindowSubsystem::Init()
    {
        glfwSetErrorCallback(ErrorCallbackHandler);
        if (!glfwInit())
        {
            ENGINE_INFO("Failed to initilize library");
            return false;
        }
        
        _window = glfwCreateWindow(_width, _height, _windowName, nullptr, nullptr);
        if (_window == nullptr)
        {
            ENGINE_INFO("Failed to create GLFW Window");
            return false;
        }
        glfwSetWindowSizeCallback(_window, WindowResizedHandler);
        glfwSetWindowUserPointer(_window, this);

        while (!glfwWindowShouldClose(_window))
        {
            glfwPollEvents();
        }
        

        return true;
    }
    
    


}
