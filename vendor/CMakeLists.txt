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
add_subdirectory(cimgui EXCLUDE_FROM_ALL)

# imgui_backends
add_library(imgui_backends
		./cimgui/imgui/backends/imgui_impl_sdl3.cpp
		./cimgui/imgui/backends/imgui_impl_opengl3.cpp)

target_include_directories(imgui_backends PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/cimgui/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/cimgui/imgui/backends
)

# Dfine vendor libr
add_library(vendor INTERFACE)

# Add to vendor target the following interfaces
target_include_directories(vendor INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/cglm/include
	${CMAKE_CURRENT_SOURCE_DIR}/imgui-backends
	${CMAKE_CURRENT_SOURCE_DIR}/cimgui/imgui
)

target_link_libraries(vendor INTERFACE SDL3::SDL3 cimgui)
