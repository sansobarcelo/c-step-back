#include "drawer.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint32_t *clear_buffer;
static uint32_t clear_width = 0;
static uint32_t clear_height = 0;
static ColorF clear_colorf;

uint32_t pack_color(ColorF color) {
  uint8_t ri = (uint8_t)(color.r * 255.0f);
  uint8_t gi = (uint8_t)(color.g * 255.0f);
  uint8_t bi = (uint8_t)(color.b * 255.0f);
  uint8_t a = 255; // fully opaque

  return (a << 24) | (ri << 16) | (gi << 8) | bi;
}

void set_pixel(Surface *surface, uint32_t x, uint32_t y, uint32_t color) {
  if (x >= 0 && x < surface->width && y >= 0 && y < surface->height) {
    surface->buffer[y * surface->width + x] = color;
  }
}

void set_clear_color(Surface *surface, ColorF color) {
  if (clear_buffer && (surface->width != clear_width || surface->height != clear_height)) {
    free(clear_buffer);
    clear_buffer = (void *)0; // Set pointer to null
  }

  if (!clear_buffer) {
    clear_buffer = malloc(sizeof(uint32_t) * surface->width * surface->height);
    clear_width = surface->width;
    clear_height = surface->height;
  }

  // generate the buffer
  printf("Recreating clear color buffer\n");
  clear_colorf = color;
  uint32_t color_packed = pack_color(color);
  for (int y = 0; y < surface->height; y++) {
    for (int x = 0; x < surface->width; x++) {
      clear_buffer[y * surface->width + x] = color_packed;
    }
  }
}

void clear_surface(Surface *surface) {
  // If size change update clear buffer with the prev color
  if (surface->width != clear_width || surface->height != clear_height) {
    set_clear_color(surface, clear_colorf);
  }
  memcpy(surface->buffer, clear_buffer, sizeof(uint32_t) * surface->width * surface->height);
}

void plot_line(Surface *surface, int x0, int y0, int x1, int y1, ColorF color) {
  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = dx + dy, e2;
  uint32_t color_packed = pack_color(color);
  while (1) {
    set_pixel(surface, x0, y0, color_packed);
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

void draw_span(Surface *surface, int y, int x0, int x1, uint32_t color) {
  // Clip Y coordinate first
  if (y < 0 || y >= surface->height)
    return;

  // Ensure x0 <= x1
  if (x0 > x1) {
    int temp = x0;
    x0 = x1;
    x1 = temp;
  }

  // Fully out-of-bounds check (left and right)
  if (x1 < 0 || x0 >= surface->width)
    return;

  // Clip X coordinates correctly
  int start_x = (x0 < 0) ? 0 : x0;
  int end_x = (x1 >= surface->width) ? surface->width - 1 : x1;

  // Ensure valid span after clipping
  if (start_x > end_x)
    return;

  // Get row pointer, ensuring we start at a valid memory location
  uint32_t *row = &surface->buffer[y * surface->width + start_x];

  // Draw the span
  for (int x = start_x; x <= end_x; x++) {
    row[x - start_x] = color;
  }
}

void draw_filled_triangle(Surface *surface, Point p0, Point p1, Point p2, uint32_t color) {
  // Sort points by Y-coordinate (lowest to highest)
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

  // Compute X slopes (avoiding divide by zero)
  float dx01 = (p1.y != p0.y) ? (float)(p1.x - p0.x) / (p1.y - p0.y) : 0;
  float dx02 = (p2.y != p0.y) ? (float)(p2.x - p0.x) / (p2.y - p0.y) : 0;
  float dx12 = (p2.y != p1.y) ? (float)(p2.x - p1.x) / (p2.y - p1.y) : 0;

  // Left-Right ordering for better rasterization
  float xa = p0.x, xb = p0.x;
  for (int y = p0.y; y < p1.y; y++) {
    draw_span(surface, y, (int)roundf(xa), (int)roundf(xb), color);
    xa += dx01;
    xb += dx02;
  }

  xa = p1.x;
  for (int y = p1.y; y < p2.y; y++) {
    draw_span(surface, y, (int)roundf(xa), (int)roundf(xb), color);
    xa += dx12;
    xb += dx02;
  }
}

void draw_thick_line(Surface *surface, Point p0, Point p1, int thickness, ColorF color) {
  if (surface->width < 1)
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
  float half_w = thickness * 0.5f;
  nx *= half_w;
  ny *= half_w;

  // Compute rectangle corners
  Point v0 = {roundf(p0.x + nx), roundf(p0.y + ny)};
  Point v1 = {roundf(p0.x - nx), roundf(p0.y - ny)};
  Point v2 = {roundf(p1.x + nx), roundf(p1.y + ny)};
  Point v3 = {roundf(p1.x - nx), roundf(p1.y - ny)};

  // Clamp corners **AFTER computing them**
  v0.x = (v0.x < 0) ? 0 : (v0.x >= surface->width ? surface->width - 1 : v0.x);
  v0.y = (v0.y < 0) ? 0 : (v0.y >= surface->height ? surface->height - 1 : v0.y);

  v1.x = (v1.x < 0) ? 0 : (v1.x >= surface->width ? surface->width - 1 : v1.x);
  v1.y = (v1.y < 0) ? 0 : (v1.y >= surface->height ? surface->height - 1 : v1.y);

  v2.x = (v2.x < 0) ? 0 : (v2.x >= surface->width ? surface->width - 1 : v2.x);
  v2.y = (v2.y < 0) ? 0 : (v2.y >= surface->height ? surface->height - 1 : v2.y);

  v3.x = (v3.x < 0) ? 0 : (v3.x >= surface->width ? surface->width - 1 : v3.x);
  v3.y = (v3.y < 0) ? 0 : (v3.y >= surface->height ? surface->height - 1 : v3.y);

  uint32_t color_packed = pack_color(color);

  // Ensure correct triangle order
  draw_filled_triangle(surface, v0, v1, v2, color_packed);
  draw_filled_triangle(surface, v1, v2, v3, color_packed);
}
