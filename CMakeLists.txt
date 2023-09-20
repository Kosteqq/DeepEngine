cmake_minimum_required(VERSION 3.20)
project(DeepEngine)

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
include_directories("${3rd_PARTY_DIR}/ImGui")

add_subdirectory("${3rd_PARTY_DIR}/glfw/")
add_subdirectory("${3rd_PARTY_DIR}/spdlog/")
set(${SPDLOG_ENABLE_PCH} ON)

add_subdirectory("${3rd_PARTY_DIR}/fmt/")
set(${INSTALL} OFF)


include(CMake/AttachVulkan.cmake)
include(CMake/AttachImGui.cmake)

include(CMake/CollectSourceFiles.cmake)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)


target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES})
target_link_libraries(${PROJECT_NAME} spdlog::spdlog_header_only)
target_link_libraries(${PROJECT_NAME} fmt)
target_link_libraries(${PROJECT_NAME} ImGui_LIB)
target_link_libraries(${PROJECT_NAME} ${VULKAN_LIB_LIST} )


# set output
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BUILD_DIR})
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BUILD_DIR})
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${BUILD_DIR})
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${BUILD_DIR})
set_property(TARGET ${PROJECT_NAME} PROPERTY EXECUTABLE_OUTPUT_PATH  ${BUILD_DIR})
