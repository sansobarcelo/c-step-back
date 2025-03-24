# SDL
set(SDL_SENSOR OFF)
set(SDL_CAMERA OFF)
set(SDL_JOYSTICK OFF)
set(SDL_HAPTIC OFF)
set(SDL_TEST OFF)
add_subdirectory(SDL EXCLUDE_FROM_ALL)

# cglm
add_subdirectory(cglm EXCLUDE_FROM_ALL)

# cimgui-backend
# add_library(cimguibackend
#     ${CMAKE_CURRENT_SOURCE_DIR}/cimgui/imgui/backends/imgui_impl_sdl3.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/cimgui/imgui/backends/imgui_impl_opengl3.cpp
# )
# target_include_directories(cimguibackend PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR}/cimgui/imgui
#     ${CMAKE_CURRENT_SOURCE_DIR}/SDL/include
# )


# Build cimgui (original repo assumed)
# Note: original cimgui uses bundled imgui
set(CIMGUI_BUILD_STATIC ON CACHE BOOL "" FORCE)
set(CIMGUI_USE_SDL3 ON CACHE BOOL "" FORCE)
set(CIMGUI_USE_OPENGL3 ON CACHE BOOL "" FORCE)

add_subdirectory(cimgui)
target_sources(cimgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/cimgui/imgui/backends/imgui_impl_sdl3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cimgui/imgui/backends/imgui_impl_opengl3.cpp
)
target_compile_definitions(cimgui PUBLIC CIMGUI_USE_OPENGL3)
target_compile_definitions(cimgui PUBLIC CIMGUI_USE_SD3)

# Define vendor
add_library(vendor INTERFACE)

target_link_libraries(vendor INTERFACE SDL3::SDL3 cimgui cglm)
