# OpenGL
find_package(OpenGL REQUIRED)

# SDL
set(SDL_SENSOR OFF)
set(SDL_CAMERA OFF)
set(SDL_JOYSTICK OFF)
set(SDL_HAPTIC OFF)
set(SDL_TEST OFF)
add_subdirectory(SDL EXCLUDE_FROM_ALL)

# cglm
add_subdirectory(cglm EXCLUDE_FROM_ALL)
if (MSVC)
  target_link_libraries(cglm PRIVATE ucrt)
endif()

# cimgui backends wrapper
add_library(cimgui_with_backends SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/cimgui/cimgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cimgui/cimgui_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cimgui/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cimgui/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cimgui/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cimgui/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cimgui/imgui/imgui_tables.cpp

    # custom backend
    ${CMAKE_CURRENT_SOURCE_DIR}/cimgui/imgui/backends/imgui_impl_sdl3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cimgui/imgui/backends/imgui_impl_opengl3.cpp

)
target_include_directories(cimgui_with_backends PUBLIC ./cimgui)
target_include_directories(cimgui_with_backends PRIVATE
    ./cimgui/imgui
    ./cimgui/imgui/backends
)

# Platform dependencies
# imgui api has: IMGUI_IMPL_API
# so here we define this to extern "C" __...
if (WIN32)
    target_compile_definitions(cimgui_with_backends PUBLIC "-DIMGUI_IMPL_API=extern \"C\" __declspec\(dllexport\)")
else(WIN32)
    target_compile_definitions(cimgui_with_backends PUBLIC "-DIMGUI_IMPL_API=extern \"C\" ")
endif(WIN32)

# On unix stdcpp needed for the cimgui build
if (UNIX)
    target_link_libraries(cimgui_with_backends PRIVATE stdc++)
endif (UNIX)

target_compile_features(cimgui_with_backends PRIVATE cxx_std_11)
target_link_libraries(cimgui_with_backends PUBLIC
    SDL3::SDL3
    OpenGL::GL
)

# Define vendor
add_library(vendor INTERFACE)
target_link_libraries(vendor INTERFACE cimgui_with_backends cglm)
