#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_log.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_surface.h>
#include <cglm/vec2.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "SDL3/SDL_opengl.h"
#include "SDL3/SDL_video.h"
#include "graphics/drawer.h"
#include "renderer.h"

#define CIMGUI_USE_OPENGL3
#define CIMGUI_USE_SDL3
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"
#include "cimgui_impl.h"
#define igGetIO igGetIO_Nil

#define WIDTH 800
#define HEIGHT 600

typedef struct {
  bool resized;
  bool running;
  uint32_t width;
  uint32_t height;
} AppState;

typedef struct {
  bool dragging;
  int last_x, last_y;
} CameraControlState;

void camera_handle_drag(Camera *camera, CameraControlState *state, int mouse_x, int mouse_y, bool mouse_down) {
  if (mouse_down) {
    if (!state->dragging) {
      state->dragging = true;
      state->last_x = mouse_x;
      state->last_y = mouse_y;
      return;
    }

    int dx = mouse_x - state->last_x;
    int dy = mouse_y - state->last_y;

    // Move camera opposite to mouse direction (screen to world)
    camera->position[0] -= dx / camera->zoom;
    camera->position[1] += dy / camera->zoom; // Y is flipped in SDL

    state->last_x = mouse_x;
    state->last_y = mouse_y;
  } else {
    state->dragging = false;
  }
}

void camera_handle_zoom(Camera *camera, float zoom_factor, int mouse_x, int mouse_y, float screen_width, float screen_height) {
  // Convert mouse position to world coordinates before zoom
  vec2 world_before;
  world_before[0] = (mouse_x - screen_width / 2.0f) / camera->zoom + camera->position[0];
  world_before[1] = (screen_height / 2.0f - mouse_y) / camera->zoom + camera->position[1];

  camera->zoom *= zoom_factor;

  // Clamp zoom (optional)
  if (camera->zoom < 0.1f)
    camera->zoom = 0.1f;
  if (camera->zoom > 10.0f)
    camera->zoom = 10.0f;

  // Convert mouse position to world coordinates after zoom
  vec2 world_after;
  world_after[0] = (mouse_x - screen_width / 2.0f) / camera->zoom + camera->position[0];
  world_after[1] = (screen_height / 2.0f - mouse_y) / camera->zoom + camera->position[1];

  // Adjust camera position so world point under cursor stays the same
  camera->position[0] += world_before[0] - world_after[0];
  camera->position[1] += world_before[1] - world_after[1];
}

// void handle_input(SDL_Event *event, Camera *camera, CameraControlState *camera_control, AppState *app_state) {
//   switch (event->type) {
//   case SDL_EVENT_MOUSE_MOTION: {
//     int mx = event->motion.x;
//     int my = event->motion.y;
//
//     // SDL_BUTTON_MIDDLE is still used in SDL3
//     uint32_t buttons = SDL_GetMouseState(NULL, NULL);
//     bool dragging = (buttons & SDL_BUTTON_MIDDLE) != 0;
//
//     if (dragging) {
//       if (!camera_control->dragging) {
//         camera_control->dragging = true;
//         camera_control->last_x = mx;
//         camera_control->last_y = my;
//         break;
//       }
//
//       int dx = mx - camera_control->last_x;
//       int dy = my - camera_control->last_y;
//
//       camera->position[0] -= dx / camera->zoom;
//       camera->position[1] += dy / camera->zoom; // SDL Y-axis is top-down
//
//       camera_control->last_x = mx;
//       camera_control->last_y = my;
//     } else {
//       camera_control->dragging = false;
//     }
//     break;
//   }
//
//   case SDL_EVENT_MOUSE_WHEEL: {
//     float mx, my;
//     SDL_GetMouseState(&mx, &my);
//
//     float zoom_factor = (event->wheel.y > 0) ? 1.1f : 0.9f;
//
//     // Convert mouse position to world coordinates before zoom
//     float screen_width = app_state->width;
//     float screen_height = app_state->height;
//
//     vec2 world_before = {(mx - screen_width / 2.0f) / camera->zoom + camera->position[0],
//                          (screen_height / 2.0f - my) / camera->zoom + camera->position[1]};
//
//     camera->zoom *= zoom_factor;
//
//     if (camera->zoom < 0.1f)
//       camera->zoom = 0.1f;
//     if (camera->zoom > 10.0f)
//       camera->zoom = 10.0f;
//
//     vec2 world_after = {(mx - screen_width / 2.0f) / camera->zoom + camera->position[0],
//                         (screen_height / 2.0f - my) / camera->zoom + camera->position[1]};
//
//     camera->position[0] += world_before[0] - world_after[0];
//     camera->position[1] += world_before[1] - world_after[1];
//
//     break;
//   }
//
//   default:
//     break;
//   }
// }

void handle_input(SDL_Event *event, Camera *camera, CameraControlState *camera_control, AppState *app_state) {
  while (SDL_PollEvent(event)) {
    ImGui_ImplSDL3_ProcessEvent(event);

    if (event->type == SDL_EVENT_QUIT) {
      app_state->running = false;
    }

    if (event->type == SDL_EVENT_KEY_DOWN) {
      switch (event->key.key) {
      case SDLK_W:
        camera->position[1] += 5;
        break;
      case SDLK_S:
        camera->position[1] -= 5;
        break;
      case SDLK_A:
        camera->position[0] -= 5;
        break;
      case SDLK_D:
        camera->position[0] += 5;
        break;
      }
    }

    switch (event->type) {
    case SDL_EVENT_MOUSE_MOTION: {
      int mx = event->motion.x;
      int my = event->motion.y;

      // SDL_BUTTON_MIDDLE is still used in SDL3
      uint32_t buttons = SDL_GetMouseState(NULL, NULL);
      bool dragging = (buttons & SDL_BUTTON_MIDDLE) != 0;

      if (dragging) {
        if (!camera_control->dragging) {
          camera_control->dragging = true;
          camera_control->last_x = mx;
          camera_control->last_y = my;
          break;
        }

        int dx = mx - camera_control->last_x;
        int dy = my - camera_control->last_y;

        camera->position[0] -= dx / camera->zoom;
        camera->position[1] -= dy / camera->zoom;

        camera_control->last_x = mx;
        camera_control->last_y = my;
      } else {
        camera_control->dragging = false;
      }
      break;
    }

    case SDL_EVENT_MOUSE_WHEEL: {
      float mx, my;
      SDL_GetMouseState(&mx, &my);

      float zoom_factor = (event->wheel.y > 0) ? 1.1f : 0.9f;

      float screen_width = app_state->width;
      float screen_height = app_state->height;

      // Convert mouse position to world coordinates BEFORE zoom
      vec2 world_before = {(mx - screen_width / 2.0f) / camera->zoom + camera->position[0],
                           (my - screen_height / 2.0f) / -camera->zoom + camera->position[1]};

      // Apply zoom
      camera->zoom *= zoom_factor;
      if (camera->zoom < 0.1f)
        camera->zoom = 0.1f;
      if (camera->zoom > 10.0f)
        camera->zoom = 10.0f;

      // Convert mouse position to world coordinates AFTER zoom
      vec2 world_after = {(mx - screen_width / 2.0f) / camera->zoom + camera->position[0],
                          (my - screen_height / 2.0f) / -camera->zoom + camera->position[1]};

      // Adjust camera position to keep world point under cursor stable
      camera->position[0] += world_before[0] - world_after[0];
      camera->position[1] += world_before[1] - world_after[1];

      break;
    }

    default:
      break;
    }

    if (event->type == SDL_EVENT_MOUSE_WHEEL) {
      if (event->wheel.y > 0)
        camera->zoom += 0.05;
      else if (event->wheel.y < 0)
        camera->zoom -= 0.05;
      // TODO: For the moment here...
      if (camera->zoom <= 0.0) {
        camera->zoom = 0.05;
      }
    }

    if (event->type == SDL_EVENT_WINDOW_RESIZED) {
      app_state->resized = true;
      app_state->width = event->window.data1;
      app_state->height = event->window.data2;
    }
  }
}

void imgui_render(AppState *app_state, SoftwareOpenGlRenderer *renderer, Camera *camera, ImGuiIO *io) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  igNewFrame();

  ImGuiViewport *viewport = igGetMainViewport();

  igSetNextWindowPos((ImVec2){0, 0}, 0, (ImVec2){0, 0});
  igSetNextWindowSize((ImVec2){(float)app_state->width, (float)app_state->height}, 0);
  ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                           ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground;

  // Fullscreen background image
  igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, (ImVec2){0, 0});
  igPushStyleVar_Float(ImGuiStyleVar_WindowBorderSize, 0.0f);
  igBegin("Background", NULL, flags);
  igImage((ImTextureID)(intptr_t)renderer->texture, (ImVec2){(float)app_state->width, (float)app_state->height}, (ImVec2){0, 1}, (ImVec2){1, 0});
  igEnd();
  igPopStyleVar(2);

  // Overlay stats
  igSetNextWindowPos((ImVec2){20, 20}, ImGuiCond_Once, (ImVec2){0, 0});
  igSetNextWindowBgAlpha(0.6f);
  ImGuiWindowFlags overlay_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
                                   ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
  igBegin("Stats", NULL, overlay_flags);
  igText("FPS: %.1f", 1.0f / io->DeltaTime);
  igText("Zoom: %.2f", camera->zoom);
  igText("Position: [%.1f, %.1f]", camera->position[0], camera->position[1]);
  igEnd();

  igRender();
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

  // Setup ImGui
  igCreateContext(NULL);
  igStyleColorsDark(NULL);
  // igGetStyle()->WindowPadding = (ImVec2){0, 0};

  ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init("#version 330");
  ImGuiIO *io = igGetIO();
  io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  // Setup app
  Camera camera = {.position = {0, 0}, .zoom = 1.0f};
  CameraControlState control = {0};
  AppState app_state = {.resized = false, .running = true, .width = WIDTH, .height = HEIGHT};
  SoftwareOpenGlRenderer renderer = renderer_create(10, 10);

  SDL_Event event;
  while (app_state.running) {
    handle_input(&event, &camera, &control, &app_state);

    if (app_state.resized) {
      printf("Resized: %d, %d\n", app_state.width, app_state.height);
      renderer_handle_resize(&renderer, app_state.width, app_state.height);
      app_state.resized = false;
      io->DisplaySize = (ImVec2){(float)app_state.width / io->DisplayFramebufferScale.x, (float)app_state.height / io->DisplayFramebufferScale.y};
    }

    // Custom renderer
    renderer_render(&renderer, &camera);

    // Render ui
    imgui_render(&app_state, &renderer, &camera, io);

    // Opengl render
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
  renderer_free(&renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  printf("End\n");
  return 0;
}
