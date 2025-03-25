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

# cimgui
set(CIMGUI_USE_SDL3 ON CACHE BOOL "" FORCE)
set(CIMGUI_USE_OPENGL3 ON CACHE BOOL "" FORCE)
add_subdirectory(cimgui)

# cimgui backends wrapper
add_library(cimgui_with_backends STATIC)
target_sources(cimgui_with_backends PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/cimgui/imgui/backends/imgui_impl_sdl3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cimgui/imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(cimgui_with_backends PUBLIC
    ./cimgui
    ./cimgui/imgui
)
target_link_libraries(cimgui_with_backends PUBLIC
    cimgui
    SDL3::SDL3
    OpenGL::GL
)

# Define vendor
add_library(vendor INTERFACE)

target_link_libraries(vendor INTERFACE OpenGL::GL SDL3::SDL3 cimgui_with_backends cglm)
