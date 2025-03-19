#include "renderer.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t pack_color(uint8_t r, uint8_t g, uint8_t b) {
  return (255 << 24) | (r << 16) | (g << 8) | b; // 0xAARRGGBB
}


void set_pixel(Surface *surface, uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b) {
  if (x >= 0 && x < surface->width&& y >= 0 && y < surface->height) {
    surface->buffer[y * surface->width + x] = pack_color(r, g, b);
  }
}

void clear_color(Surface *surface, uint8_t r, uint8_t g, uint8_t b) {
  printf("width: %d, height: %d\n", surface->width, surface->height);
  for (int y = 0; y < surface->height; y++) {
    for (int x = 0; x < surface->width; x++) {
      set_pixel(surface, x, y, r, g, b);
    }
  }
  printf("width: %d, height: %d\n", surface->width, surface->height);
}

// void fill_span(Surface *surface, int y, int x_start, int x_end, uint8_t r, uint8_t g, uint8_t b) {
//   if (y < 0 || y >= surface->height)
//     return; // Clipping
//   if (x_start > x_end)
//     swap(&x_start, &x_end);
//
//   if (x_end < 0 || x_start >= surface->width)
//     return; // Clipping
//
//   for (int x = (x_start < 0 ? 0 : x_start); x <= (x_end >= surface->width ? surface->width - 1 : x_end); x++) {
//     set_pixel(surface, x, y, r, g, b);
//   }
// }

void plot_line(Surface *surface, int x0, int y0, int x1, int y1, int r, int g, int b) {
  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = dx + dy, e2;

  while (1) {
    set_pixel(surface, x0, y0, r, g, b);
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
  if (y < 0 || y >= surface->height || x0 >= surface->width || x1 < 0)
    return;

  if (x0 > x1) {
    int temp = x0;
    x0 = x1;
    x1 = temp;
  } // Ensure x0 < x1
  if (x0 < 0)
    x0 = 0;
  if (x1 >= surface->width)
    x1 = surface->width - 1;

  uint32_t *row = &surface->buffer[y * surface->width + x0];
  for (int x = x0; x <= x1; x++) {
    row[x - x0] = color;
  }
}

void draw_filled_triangle(Surface *surface, Point p0, Point p1, Point p2, uint32_t color) {
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
    draw_span(surface, y, (int)xa, (int)xb, color);
    xa += dx01;
    xb += dx02;
  }

  xa = p1.x;
  for (int y = p1.y; y < p2.y; y++) {
    draw_span(surface, y, (int)xa, (int)xb, color);
    xa += dx12;
    xb += dx02;
  }
}

void draw_thick_line(Surface *surface, Point p0, Point p1, int thickness, uint8_t r, uint8_t g, uint8_t b) {
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
  Point v0 = {p0.x + (int)nx, p0.y + (int)ny};
  Point v1 = {p0.x - (int)nx, p0.y - (int)ny};
  Point v2 = {p1.x + (int)nx, p1.y + (int)ny};
  Point v3 = {p1.x - (int)nx, p1.y - (int)ny};

  uint32_t color = pack_color(r, g, b);

  draw_filled_triangle(surface, v0, v1, v2, color);
  draw_filled_triangle(surface, v1, v2, v3, color);
}
