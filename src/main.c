#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_log.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_surface.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "cglm/vec2.h"
#include "graphics/renderer.h"

#define WIDTH 800
#define HEIGHT 600

typedef struct {
  vec2 position;
  double zoom;
} Camera;

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
  vec2 p0 = {0, 0};
  vec2 p1 = {100, 0};

  glm_vec2_scale(p0, camera->zoom, p0);
  glm_vec2_scale(p1, camera->zoom, p1);

  glm_vec2_sub(p0, camera->position, p0);
  glm_vec2_sub(p1, camera->position, p1);

  draw_thick_line(surface, (Point){p0[0], p0[1]}, (Point){p1[0], p1[1]}, 25, 10, 244, 10);
}

void handle_input(SDL_Event *event, Camera *camera) {
  if (event->type == SDL_EVENT_MOUSE_MOTION) {
    // mouse_pos.x = event->motion.x;
    // mouse_pos.y = event->motion.y;
  }


  if (event->type == SDL_EVENT_KEY_DOWN) {
    if (event->key.key == SDLK_UP) {
      camera->position[1] -= 5;
    }

    if (event->key.key == SDLK_DOWN) {
      camera->position[1] += 5;
    }


    if (event->key.key == SDLK_LEFT) {
      camera->position[0] -= 5;
    }

    if (event->key.key == SDLK_RIGHT) {
      camera->position[0] += 5;
    }

    if (event->key.key == SDLK_A) {
      camera->zoom -= 0.01;
    }

    if (event->key.key == SDLK_S) {
      camera->zoom += 0.01;
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

int main() {
  Camera camera = {
      .position = {0, 0},
      .zoom = 1.0,
  };
  Surface surface = create_surface(WIDTH, HEIGHT);

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
    return -1;
  }

  SDL_Window *window = SDL_CreateWindow("Software Renderer", surface.width, surface.height, SDL_WINDOW_HIDDEN);
  if (!window) {
    SDL_Log("Window could not be created! SDL_Error: %s", SDL_GetError());
    SDL_Quit();
    return -1;
  }

  SDL_Surface *screenSurface = SDL_GetWindowSurface(window);
  if (!screenSurface) {
    SDL_Log("Could not get window surface! SDL_Error: %s", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return -1;
  }

  SDL_Surface *framebufferSurface = SDL_CreateSurface(surface.width, surface.height, SDL_PIXELFORMAT_ARGB8888);

  if (!framebufferSurface) {
    SDL_Log("Could not create framebuffer surface! SDL_Error: %s", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return -1;
  }

  if (!SDL_ShowWindow(window))
    return -1;

  int running = 1;
  SDL_Event event;
  while (running) {
    // Indirect
    SDL_WaitEvent(&event);
    if (event.type == SDL_EVENT_QUIT) {
      running = 0;
    }
    handle_input(&event, &camera);

    // Direct
    // while (SDL_PollEvent(&event)) {
    //   if (event.type == SDL_EVENT_QUIT) {
    //     running = 0;
    //   }
    // }

    render_scene(&camera, &surface); // Draw to framebuffer

    // Copy framebuffer data to SDL surface
    memcpy(framebufferSurface->pixels, surface.buffer, surface.width * surface.height * sizeof(uint32_t));
    SDL_BlitSurface(framebufferSurface, NULL, screenSurface, NULL);
    SDL_UpdateWindowSurface(window);
  }

  SDL_DestroySurface(framebufferSurface);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
