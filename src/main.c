#include "SDL3/SDL_log.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_surface.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 800
#define HEIGHT 400

typedef struct {
  int x, y;
} Point;

// Framebuffer (32-bit ARGB)
uint32_t framebuffer[WIDTH * HEIGHT];
Point mouse_pos;

// Function to create a 32-bit ARGB color
uint32_t pack_color(int r, int g, int b) {
  return (255 << 24) | (r << 16) | (g << 8) | b; // 0xAARRGGBB
}

// Function to set a pixel in the framebuffer
void set_pixel(int x, int y, int r, int g, int b) {
  if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
    framebuffer[y * WIDTH + x] = pack_color(r, g, b);
  }
}

// Draw a simple gradient
void draw_gradient() {
  for (int y = 0; y < HEIGHT; y++) {
    int color_value = (y * 255) / HEIGHT;
    for (int x = 0; x < WIDTH; x++) {
      // int r = (x * 255) / WIDTH;
      // int g = (y * 255) / HEIGHT;
      // int b = 128; // Fixed blue tone
      int r = color_value;
      int g = color_value;
      int b = color_value;
      set_pixel(x, y, r, g, b);
    }
  }
}

void clear_color(int r, int g, int b) {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      set_pixel(x, y, r, g, b);
    }
  }
}

// Save framebuffer to a PPM file (convert `uint32_t` to RGB)
void save_ppm(const char *filename) {
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
    uint32_t color = framebuffer[i];
    uint8_t rgb[3] = {
        (color >> 16) & 0xFF, // Extract Red
        (color >> 8) & 0xFF,  // Extract Green
        (color) & 0xFF        // Extract Blue
    };
    fwrite(rgb, 1, 3, f);
  }
  fclose(f);
}

void swap(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}

void fill_span(int y, int x_start, int x_end, uint8_t r, uint8_t g, uint8_t b) {
  if (y < 0 || y >= HEIGHT)
    return; // Clipping
  if (x_start > x_end)
    swap(&x_start, &x_end);

  if (x_end < 0 || x_start >= WIDTH)
    return; // Clipping

  for (int x = (x_start < 0 ? 0 : x_start); x <= (x_end >= WIDTH ? WIDTH - 1 : x_end); x++) {
    set_pixel(x, y, r, g, b);
  }
}

// Plot line
void plot_line(int x0, int y0, int x1, int y1, int r, int g, int b) {
  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = dx + dy, e2;

  while (1) {
    set_pixel(x0, y0, r, g, b);
    if (x0 == x1 && y0 == y1)
      break; // Exit condition

    e2 = 2 * err;
    if (e2 >= dy) {
      err += dy;
      x0 += sx;
    } // Move in x direction
    if (e2 <= dx) {
      err += dx;
      y0 += sy;
    } // Move in y direction
  }
}

// Draw a horizontal span (fast memory access)
void draw_span(int y, int x0, int x1, uint32_t color) {
  if (y < 0 || y >= HEIGHT || x0 >= WIDTH || x1 < 0)
    return;

  if (x0 > x1) {
    int temp = x0;
    x0 = x1;
    x1 = temp;
  } // Ensure x0 < x1
  if (x0 < 0)
    x0 = 0;
  if (x1 >= WIDTH)
    x1 = WIDTH - 1;

  uint32_t *row = &framebuffer[y * WIDTH + x0];
  for (int x = x0; x <= x1; x++) {
    row[x - x0] = color;
  }
}

// Rasterize a filled triangle (split for scanline rendering)
void draw_filled_triangle(Point p0, Point p1, Point p2, uint32_t color) {
  // Sort points by y-coordinate
  if (p1.y < p0.y) {
    Point tmp = p0;
    p0 = p1;
    p1 = tmp;
  }
  if (p2.y < p0.y) {
    Point tmp = p0;
    p0 = p2;
    p2 = tmp;
  }
  if (p2.y < p1.y) {
    Point tmp = p1;
    p1 = p2;
    p2 = tmp;
  }

  float dx01 = (p1.y - p0.y) ? (float)(p1.x - p0.x) / (p1.y - p0.y) : 0;
  float dx02 = (p2.y - p0.y) ? (float)(p2.x - p0.x) / (p2.y - p0.y) : 0;
  float dx12 = (p2.y - p1.y) ? (float)(p2.x - p1.x) / (p2.y - p1.y) : 0;

  float xa = p0.x, xb = p0.x;
  for (int y = p0.y; y < p1.y; y++) {
    draw_span(y, (int)xa, (int)xb, color);
    xa += dx01;
    xb += dx02;
  }

  xa = p1.x;
  for (int y = p1.y; y < p2.y; y++) {
    draw_span(y, (int)xa, (int)xb, color);
    xa += dx12;
    xb += dx02;
  }
}

// Draw a thick line as a rectangle
void draw_thick_line(Point p0, Point p1, int width, uint8_t r, uint8_t g, uint8_t b) {
  if (width < 1)
    return;

  // Compute direction vector
  float dx = p1.x - p0.x;
  float dy = p1.y - p0.y;
  float length = sqrtf(dx * dx + dy * dy);
  if (length == 0)
    return;

  // Normalize and find perpendicular
  float nx = -dy / length;
  float ny = dx / length;

  // Scale by half width
  float half_w = width * 0.5f;
  nx *= half_w;
  ny *= half_w;

  // Compute rectangle corners
  Point v0 = {p0.x + (int)nx, p0.y + (int)ny};
  Point v1 = {p0.x - (int)nx, p0.y - (int)ny};
  Point v2 = {p1.x + (int)nx, p1.y + (int)ny};
  Point v3 = {p1.x - (int)nx, p1.y - (int)ny};

  uint32_t color = pack_color(r, g, b);

  // Draw as two triangles
  draw_filled_triangle(v0, v1, v2, color);
  draw_filled_triangle(v1, v2, v3, color);
}

void render_scene() {
  clear_color(0, 0, 0);
  // plot_line(100, 100, 300, 300, 10, 244, 10);
  // plot_line(300, 500, 500, 500, 10, 244, 10);
  // plot_line(550, 500, 700, 300, 10, 244, 10);
  draw_thick_line((Point){250, 250}, (Point){mouse_pos.x, mouse_pos.y}, 15, 10, 244, 10);
}

void handle_input(SDL_Event *event) {
  if (event->type == SDL_EVENT_MOUSE_MOTION) {
    mouse_pos.x = event->motion.x;
    mouse_pos.y = event->motion.y;
  }
}

int main() {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
    return -1;
  }

  SDL_Window *window = SDL_CreateWindow("Software Renderer", WIDTH, HEIGHT, SDL_WINDOW_HIDDEN);
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

  SDL_Surface *framebufferSurface = SDL_CreateSurface(WIDTH, HEIGHT, SDL_PIXELFORMAT_ARGB8888);

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
    handle_input(&event);

    // Direct
    // while (SDL_PollEvent(&event)) {
    //   if (event.type == SDL_EVENT_QUIT) {
    //     running = 0;
    //   }
    // }

    render_scene(); // Draw to framebuffer

    // Copy framebuffer data to SDL surface
    memcpy(framebufferSurface->pixels, framebuffer, WIDTH * HEIGHT * sizeof(uint32_t));

    SDL_BlitSurface(framebufferSurface, NULL, screenSurface, NULL);
    SDL_UpdateWindowSurface(window);
  }

  SDL_DestroySurface(framebufferSurface);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
