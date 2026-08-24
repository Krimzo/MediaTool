cmake_minimum_required(VERSION 4.4.0)
project(MediaTool LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB imgui_sources CONFIGURE_DEPENDS imgui/*.cpp)
add_library(imguilib
    ${imgui_sources}
    imgui/backends/imgui_impl_win32.cpp
    imgui/backends/imgui_impl_dx11.cpp
    imgui/misc/cpp/imgui_stdlib.cpp
)
target_include_directories(imguilib PUBLIC imgui)

add_subdirectory(klibrary)
target_include_directories(klibrarylib PUBLIC imgui)

file(GLOB_RECURSE mediatool_sources CONFIGURE_DEPENDS source/*.cpp)
add_executable(mediatool ${mediatool_sources})
target_link_libraries(mediatool PUBLIC imguilib)
target_link_libraries(mediatool PUBLIC klibrarylib)
target_include_directories(mediatool PUBLIC source)
set_target_properties(mediatool PROPERTIES
    DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
