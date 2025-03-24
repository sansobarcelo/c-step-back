#include "SDL3/SDL_hints.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_log.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_surface.h>
#include <cglm/vec2.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "SDL3/SDL_opengl.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_video.h"
#include "camera.h"
#include "graphics/renderer.h"

#define CIMGUI_USE_OPENGL3
#define CIMGUI_USE_SDL3
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"
#include "cimgui_impl.h"
#define igGetIO igGetIO_Nil

#define WIDTH 800
#define HEIGHT 600

// Save framebuffer to a PPM file (convert `uint32_t` to RGB)
void save_ppm(const char *filename, Surface *surface) {
  FILE *f;
  f = fopen(filename, "wb");
  if (!f) {
    perror("Failed to open file");
    return;
  }

  // Write PPM header
  fprintf(f, "P6\n%d %d\n255\n", WIDTH, HEIGHT);

  // Write pixel data (convert 32-bit ARGB to 24-bit RGB)
  for (int i = 0; i < WIDTH * HEIGHT; i++) {
    uint32_t color = surface->buffer[i];
    uint8_t rgb[3] = {
        (color >> 16) & 0xFF, // Extract Red
        (color >> 8) & 0xFF,  // Extract Green
        (color) & 0xFF        // Extract Blue
    };
    fwrite(rgb, 1, 3, f);
  }
  fclose(f);
}

void render_scene(Camera *camera, Surface *surface) {
  clear_color(surface, 100, 30, 10);

  // Object example
  vec2 p0 = {0, 0};
  vec2 p1 = {100, 0};
  world_to_screen(p0, p0, camera, surface->width, surface->height);
  world_to_screen(p1, p1, camera, surface->width, surface->height);

  draw_thick_line(surface, (Point){p0[0], p0[1]}, (Point){p1[0], p1[1]}, 25, 10, 244, 10);
}
void handle_input(SDL_Event *event, Camera *camera, bool *running) {
  while (SDL_PollEvent(event)) {
    ImGui_ImplSDL3_ProcessEvent(event);

    if (event->type == SDL_EVENT_QUIT) {
      *running = false;
    }

    if (event->type == SDL_EVENT_KEY_DOWN) {
      switch (event->key.key) {
      case SDLK_UP:
        camera->position[1] += 5;
        break;
      case SDLK_DOWN:
        camera->position[1] -= 5;
        break;
      case SDLK_LEFT:
        camera->position[0] -= 5;
        break;
      case SDLK_RIGHT:
        camera->position[0] += 5;
        break;
      }
    }

    if (event->type == SDL_EVENT_MOUSE_WHEEL) {
      if (event->wheel.y > 0)
        camera->zoom += 1;
      else if (event->wheel.y < 0)
        camera->zoom -= 1;
    }
  }
}

Surface create_surface(uint32_t width, uint32_t height) {
  return (Surface){
      .width = width,
      .height = height,
      .buffer = malloc(width * height * sizeof(uint32_t)),
  };
}

void update_texture(GLuint texture, const Surface *surface) {
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, surface->width, surface->height, GL_BGRA, GL_UNSIGNED_BYTE, surface->buffer);
}

GLuint create_texture_from_surface(const Surface *surface) {
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Prevent blurring
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->width, surface->height, 0, GL_BGRA, GL_UNSIGNED_BYTE, surface->buffer);
  return texture;
}

int main() {

  if (SDL_Init(SDL_INIT_VIDEO) == 0) {
    SDL_Log("SDL_Init failed: %s", SDL_GetError());
    return -1;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  SDL_Window *window = SDL_CreateWindow("Software Renderer + ImGui", WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  SDL_GL_MakeCurrent(window, gl_context);
  SDL_GL_SetSwapInterval(1); // vsync

  // Create surface
  Surface surface = create_surface(WIDTH, HEIGHT);
  GLuint cpu_texture = create_texture_from_surface(&surface);

  // Setup camera
  Camera camera = {.position = {0, 0}, .zoom = 1.0f};

  // Setup ImGui
  igCreateContext(NULL);
  igStyleColorsDark(NULL);
  ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init("#version 330");
  ImGuiIO *io = igGetIO();
  io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  bool running = true;
  SDL_Event event;

  while (running) {
    handle_input(&event, &camera, &running);
    render_scene(&camera, &surface);
    update_texture(cpu_texture, &surface);

    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    igNewFrame();

    // Get screen size from ImGui
    ImGuiViewport *viewport = igGetMainViewport();
    igSetNextWindowPos(viewport->Pos, 0, (ImVec2){0, 0});
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                             ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground;

    // Background
    igBegin("Background", NULL, flags);
    igImage((ImTextureID)(intptr_t)cpu_texture, viewport->Size, (ImVec2){0, 1}, (ImVec2){1, 0});

    // Floating overlay window example
    igSetNextWindowPos((ImVec2){20, 20}, ImGuiCond_Once, (ImVec2){0, 0});
    igSetNextWindowBgAlpha(0.6f); // Transparent background

    ImGuiWindowFlags overlay_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
                                     ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

    igBegin("Stats", NULL, overlay_flags);
    igText("FPS: %.1f", 1.0f / io->DeltaTime);
    igText("Zoom: %.2f", camera.zoom);
    igText("Position: [%.1f, %.1f]", camera.position[0], camera.position[1]);
    igEnd();

    igEnd();

    igRender();
    glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
    SDL_GL_SwapWindow(window);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  igDestroyContext(NULL);
  glDeleteTextures(1, &cpu_texture);
  // destroy_surface(&surface);
  // SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
