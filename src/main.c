#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_log.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_surface.h>
#include <cglm/vec2.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "SDL3/SDL_timer.h"
#include "camera.h"
#include "graphics/renderer.h"

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

void handle_input(SDL_Event *event, Camera *camera) {
  if (event->type == SDL_EVENT_MOUSE_MOTION) {
    // mouse_pos.x = event->motion.x;
    // mouse_pos.y = event->motion.y;
  }

  if (event->type == SDL_EVENT_KEY_DOWN) {
    if (event->key.key == SDLK_UP) {
      camera->position[1] += 5;
    }

    if (event->key.key == SDLK_DOWN) {
      camera->position[1] -= 5;
    }

    if (event->key.key == SDLK_LEFT) {
      camera->position[0] -= 5;
    }

    if (event->key.key == SDLK_RIGHT) {
      camera->position[0] += 5;
    }

  } else if (event->type == SDL_EVENT_MOUSE_WHEEL) {
    if (event->wheel.y > 0) {
      camera->zoom += 1;
    } else if (event->wheel.y < 0) {
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

  const int FPS_CAP = 60;
  const int FRAME_DELAY = 1000 / FPS_CAP; // 1000ms divided by 60 FPS = ~16.67ms per frame

  uint32_t lastFrameTime = SDL_GetTicks(); // Track time of last frame
  while (running) {
    uint32_t frameStart = SDL_GetTicks(); // Get current time at frame start

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

    // Only render if enough time has passed for 60 FPS
    if (SDL_GetTicks() - lastFrameTime >= FRAME_DELAY) {
      lastFrameTime = SDL_GetTicks(); // Update last render time

      render_scene(&camera, &surface); // Draw to framebuffer

      // Copy framebuffer data to SDL surface
      memcpy(framebufferSurface->pixels, surface.buffer, surface.width * surface.height * sizeof(uint32_t));
      SDL_BlitSurface(framebufferSurface, NULL, screenSurface, NULL);
      SDL_UpdateWindowSurface(window);
    }

    // Frame delay to keep 60 FPS stable
    uint32_t frameTime = SDL_GetTicks() - frameStart;
    if (frameTime < FRAME_DELAY) {
      SDL_Delay(FRAME_DELAY - frameTime); // Delay to match 60 FPS
    }
  }

  SDL_DestroySurface(framebufferSurface);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
