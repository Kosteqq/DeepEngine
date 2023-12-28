set(IMGUI_SOURCE
    ../3rdParty/ImGui/imconfig.h
    ../3rdParty/ImGui/imgui_demo.cpp
    ../3rdParty/ImGui/imgui_draw.cpp
    ../3rdParty/ImGui/imgui_internal.h
    ../3rdParty/ImGui/imgui_tables.cpp
    ../3rdParty/ImGui/imgui_widgets.cpp
    ../3rdParty/ImGui/imgui.cpp
    ../3rdParty/ImGui/imgui.h
    ../3rdParty/ImGui/backends/imgui_impl_glfw.h
    ../3rdParty/ImGui/backends/imgui_impl_glfw.cpp
    ../3rdParty/ImGui/backends/imgui_impl_vulkan.h
    ../3rdParty/ImGui/backends/imgui_impl_vulkan.cpp)

add_library(ImGui_LIB STATIC ${IMGUI_SOURCE})
set_property(TARGET ImGui_LIB PROPERTY CXX_STANDARD 20)
