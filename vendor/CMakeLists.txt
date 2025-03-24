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
add_subdirectory(cimgui)
# add desired backend
target_sources(cimgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/cimgui/imgui/backends/imgui_impl_sdl3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cimgui/imgui/backends/imgui_impl_opengl3.cpp
)

# Define vendor
add_library(vendor INTERFACE)

target_link_libraries(vendor INTERFACE SDL3::SDL3 cimgui cglm)
