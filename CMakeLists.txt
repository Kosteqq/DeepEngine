cmake_minimum_required(VERSION 3.20)
project(DeepEngine CXX)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/DeepEngine")

set(BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Build")
set(3rd_PARTY_DIR "3rdParty")

if(MSVC)
    # Multithread compilation
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    # Show linking files
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /LTCG:STATUS")
endif()

include_directories("${SOURCE_DIR}/")
include_directories("${3rd_PARTY_DIR}/glfw/include")
include_directories("${3rd_PARTY_DIR}/spdlog/include")
include_directories("${3rd_PARTY_DIR}/glm")
include_directories("${3rd_PARTY_DIR}/ImGui")
include_directories("${3rd_PARTY_DIR}/yaml-cpp/include")

add_subdirectory("${3rd_PARTY_DIR}/glfw/")
set(SPDLOG_ENABLE_PCH ON)
add_subdirectory("${3rd_PARTY_DIR}/spdlog/")

set(YAML_CPP_BUILD_TESTS OFF)
set(YAML_CPP_BUILD_TOOLS OFF)
add_subdirectory("${3rd_PARTY_DIR}/yaml-cpp")

set(INSTALL OFF)
add_subdirectory("${3rd_PARTY_DIR}/fmt/")

include(CMake/AttachVulkan.cmake)
include(CMake/AttachImGui.cmake)

include(CMake/CollectSourceFiles.cmake)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

# Disable warnings
target_compile_definitions(${PROJECT_NAME} PRIVATE _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)

target_precompile_headers(${PROJECT_NAME} PRIVATE "${SOURCE_DIR}/Engine/Renderer/Vulkan/VulkanPCH.h")
target_compile_options(${PROJECT_NAME} PRIVATE /Yu "${SOURCE_DIR}/Engine/Renderer/Vulkan/VulkanPCH.h")

target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES})
target_link_libraries(${PROJECT_NAME} spdlog::spdlog_header_only)
target_link_libraries(${PROJECT_NAME} fmt)
target_link_libraries(${PROJECT_NAME} ImGui_LIB)
target_link_libraries(${PROJECT_NAME} yaml-cpp)
target_link_libraries(${PROJECT_NAME} ${VULKAN_LIB_LIST} )

# set output
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BUILD_DIR})
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BUILD_DIR})
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${BUILD_DIR})
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${BUILD_DIR})
set_property(TARGET ${PROJECT_NAME} PROPERTY EXECUTABLE_OUTPUT_PATH  ${BUILD_DIR})
