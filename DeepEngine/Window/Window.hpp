#pragma once

//#define GLFW_INCLUDE_VULKAN
#include <string>
#include <GLFW/glfw3.h>

namespace DeepEngine
{
    class Window
    {
        
    public:
        Window(int p_width, int p_height, std::string p_name);
        ~Window();
        
    private:

        void InitWindow();
        static void ErrorCallbackHandler(int error, const char* description);

        int _width;
        int _height;

        std::string _windowName;
        GLFWwindow* window;
        
    };
}
