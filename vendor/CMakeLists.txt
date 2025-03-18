# SDL
set(SDL_SENSOR OFF)
set(SDL_CAMERA OFF)
set(SDL_JOYSTICK OFF)
set(SDL_HAPTIC OFF)
set(SDL_TEST OFF)
add_subdirectory(SDL EXCLUDE_FROM_ALL)

# Dfine vendor libr
add_library(vendor INTERFACE)

# Add to vendor target the following interfaces
# target_include_directories(vendor INTERFACE ./cglm/include/)
target_link_libraries(vendor INTERFACE SDL3::SDL3)
