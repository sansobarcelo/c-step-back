add_executable(main)

target_sources(main PRIVATE main.c renderer.c canvas.c draw_context.c graphics/drawer.c) #app.c renderer/custom.c utils/vector.c)
target_link_libraries(main PRIVATE vendor)

if(NOT WIN32)
    target_link_libraries(main PRIVATE m)
endif()

# Custom command to copy assets
# set(ASSETS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
# set(ASSETS_DEST_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}assets")
