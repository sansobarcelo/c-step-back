#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include <cglm/vec2.h>

#include "components.h"
#include "flecs.h"
#include "flecs/addons/flecs_c.h"
#include "renderer.h"

#define CIMGUI_USE_OPENGL3
#define CIMGUI_USE_SDL3
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"
#include "cimgui_impl.h"
#define igGetIO igGetIO_Nil

#define CANVAS_MIN_SCALE 0.1f
#define CANVAS_MAX_SCALE 10.0f

#define WIDTH 800
#define HEIGHT 600

typedef struct {
  bool resized;
  bool running;
  uint32_t width;
  uint32_t height;

  ecs_world_t *world;

  bool show_debug;
} AppState;

typedef struct {
  bool dragging;
  int last_x, last_y;
} InputState;

// Apply zoom scale with clamping
void canvas_apply_zoom(Canvas *canvas, float zoom_factor) {
  float new_scale = canvas->scale * zoom_factor;
  if (new_scale < CANVAS_MIN_SCALE)
    new_scale = CANVAS_MIN_SCALE;
  if (new_scale > CANVAS_MAX_SCALE)
    new_scale = CANVAS_MAX_SCALE;
  canvas->scale = new_scale;
}

void canvas_handle_drag(Canvas *canvas, InputState *state, int mouse_x, int mouse_y, bool mouse_down) {
  if (mouse_down) {
    if (!state->dragging) {
      state->dragging = true;
      state->last_x = mouse_x;
      state->last_y = mouse_y;
      return;
    }

    int dx = mouse_x - state->last_x;
    int dy = mouse_y - state->last_y;

    // Apply scale to convert screen-space delta to world-space movement
    float sensitivity = 0.4f; // tweak this value
    float world_dx = -dx * powf(canvas->scale, sensitivity);
    float world_dy = dy * powf(canvas->scale, sensitivity); // flip Y

    canvas_translate(canvas, world_dx, world_dy);

    state->last_x = mouse_x;
    state->last_y = mouse_y;
  } else {
    state->dragging = false;
  }
}

void canvas_handle_zoom(Canvas *canvas, float zoom_factor, int mouse_x, int mouse_y) {
  vec2 screen = {mouse_x, mouse_y};
  vec2 world_before, world_after;

  canvas_screen_to_world(canvas, screen, world_before);
  canvas_apply_zoom(canvas, zoom_factor);
  canvas_screen_to_world(canvas, screen, world_after);

  // Offset canvas so zoom appears centered on cursor
  canvas_translate(canvas, world_before[0] - world_after[0], world_before[1] - world_after[1]);
}

void handle_input(SDL_Event *event, Canvas *canvas, InputState *input_state, AppState *app_state) {
  while (SDL_PollEvent(event)) {
    ImGui_ImplSDL3_ProcessEvent(event);

    if (event->type == SDL_EVENT_QUIT) {
      app_state->running = false;
    }

    if (event->type == SDL_EVENT_KEY_DOWN) {
      switch (event->key.key) {
      case SDLK_F1:
        app_state->show_debug = !app_state->show_debug;
        break;
      case SDLK_W:
        // app_state->line.transform.position[1] += 10;
        break;
      case SDLK_S:
        // app_state->line.transform.position[1] -= 10;
        break;
      case SDLK_A:
        // app_state->line.transform.position[0] -= 10;
        break;
      case SDLK_D:
        // app_state->line.transform.position[0] += 10;
        break;
      }
    }

    switch (event->type) {
    case SDL_EVENT_MOUSE_MOTION: {
      int mx = event->motion.x;
      int my = event->motion.y;

      uint32_t buttons = SDL_GetMouseState(NULL, NULL);
      bool dragging = (buttons & SDL_BUTTON_MIDDLE) != 0;

      if (dragging) {
        if (!input_state->dragging) {
          input_state->dragging = true;
          input_state->last_x = mx;
          input_state->last_y = my;
          break;
        }

        canvas_handle_drag(canvas, input_state, mx, my, true);

        input_state->last_x = mx;
        input_state->last_y = my;
      } else {
        input_state->dragging = false;
      }
      break;
    }
    case SDL_EVENT_MOUSE_WHEEL: {
      float mx, my;
      SDL_GetMouseState(&mx, &my);

      float screen_width = app_state->width;
      float screen_height = app_state->height;
      float zoom_factor = SDL_powf(1.1f, event->wheel.y);

      canvas_handle_zoom(canvas, zoom_factor, mx, my);
      break;
    }

    case SDL_EVENT_MOUSE_BUTTON_UP: {
      break;
    }

    default:
      break;
    }

    if (event->type == SDL_EVENT_WINDOW_RESIZED) {
      app_state->resized = true;
      app_state->width = event->window.data1;
      app_state->height = event->window.data2;
    }
  }
}

void imgui_render(AppState *app_state, SoftwareOpenGlRenderer *renderer, ImGuiIO *io) {
  Canvas *canvas = &renderer->draw_context.canvas;
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

  // Left Panel (Fullscreen Vertical)
  if (app_state->show_debug) {
    ImGuiViewport *viewport = igGetMainViewport();
    ImVec2 panel_pos = viewport->Pos;
    ImVec2 panel_size = {300, viewport->Size.y};

    igSetNextWindowPos(panel_pos, ImGuiCond_None, (ImVec2){0.0f, 0.0f});
    igSetNextWindowSize(panel_size, ImGuiCond_None);

    igBegin("Debug Panel", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    igText("FPS: %.1f", 1.0f / io->DeltaTime);
    igText("Frame time %.2f ms/frame", 1000.0f / igGetIO()->Framerate);
    igSeparator();

    igText("Canvas Zoom: %.2f", canvas->scale);
    igText("Canvas Position: [%.1f, %.1f]", canvas->position[0], canvas->position[1]);
    igSeparator();

    // Line data
    // Line *line = &app_state->line;
    // igText("Line world pos: [%.1f, %.1f]", line->transform.position[0], line->transform.position[1]);
    igSeparator();

    // BG color picker
    igText("Clear color");
    // TODO: Create state
    static float bg_color[4] = {0};
    if (igColorEdit4("Color", bg_color, ImGuiColorEditFlags_None)) {
      ColorF bg_colorf = {
          .r = bg_color[0],
          .g = bg_color[1],
          .b = bg_color[2],
          .a = bg_color[3],
      };
      renderer_set_clear_color(renderer, bg_colorf);
    }

    igEnd();
  }

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

  ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init("#version 330");
  ImGuiIO *io = igGetIO();
  io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  // Setup world
  // ecs_log_set_level(1);
  ecs_world_t *world = ecs_init();

  // Register component types
  ECS_COMPONENT(world, Line);
  ECS_COMPONENT(world, Position);

  // Setup app
  InputState control = {0};
  AppState app_state = {.resized = true, .running = true, .width = WIDTH, .height = HEIGHT, .show_debug = true, .world = world};
  SoftwareOpenGlRenderer renderer = renderer_create(WIDTH, HEIGHT);

  // Start bg_color
  renderer_set_clear_color(&renderer, (ColorF){0});

  // To test
  renderer.draw_context.canvas.position[0] += 100;
  ecs_entity_t e1 = ecs_new(world);
  ecs_set(world, e1, Position, {.pos_x = 10.0f, .pos_y = 20.0f});
  ecs_set(world, e1, Line, {.ax = 0.0f, .ay = 0.0f, .bx = 100.0f, .by = 0.0f, .thickness = 10.0f});

  // Create the query
  ecs_query_t *line_transform_q = ecs_query(world, {.terms = {{ecs_id(Line)}, {ecs_id(Position)}}});

  SDL_Event event;
  while (app_state.running) {
    handle_input(&event, &renderer.draw_context.canvas, &control, &app_state);

    if (app_state.resized) {
      printf("Resized: %d, %d\n", app_state.width, app_state.height);
      renderer_handle_resize(&renderer, app_state.width, app_state.height);
      app_state.resized = false;
      io->DisplaySize = (ImVec2){(float)app_state.width / io->DisplayFramebufferScale.x, (float)app_state.height / io->DisplayFramebufferScale.y};
    }

    // Custom renderer
    renderer_render(&renderer, app_state.world, line_transform_q);

    // Render ui
    imgui_render(&app_state, &renderer, io);

    // Opengl render
    glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
    SDL_GL_SwapWindow(window);
  }

  // Cleanup
  ecs_fini(app_state.world);
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  igDestroyContext(NULL);
  renderer_free(&renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  printf("End\n");
  return 0;
}
