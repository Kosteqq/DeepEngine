#include "Window.hpp"
#include "../Debug/Debug.h"

namespace DeepEngine
{
    Window::Window(int p_width, int p_height, std::string p_name) : _width{p_width}, _height{p_height}, _windowName{p_name}
    {
        InitWindow();
    }

    Window::~Window()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
    
    void Window::ErrorCallbackHandler(int error, const char* description)
    {
        fprintf(stderr, "Error: %s\n", description);
    }
    
    void Window::InitWindow()
    {
        glfwSetErrorCallback(ErrorCallbackHandler);

        window = glfwCreateWindow(_width, _height, _windowName.c_str(), nullptr, nullptr);
        if (!glfwInit())
        {
            LOG("Failed to initilize library")
        }
    }
}
