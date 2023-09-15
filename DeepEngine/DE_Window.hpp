#pragma once

#include <string>


namespace DeepEngine
{
    class DEWindow
    {
        
    public:
        DEWindow(int p_width, int p_height, std::string p_name);
        ~DEWindow();
        
    private:

        void InitWindow();

        int _Width;
        int _Height;

        std::string _WindowName;
        
    };
}
