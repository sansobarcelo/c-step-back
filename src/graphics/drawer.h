#pragma once

#include "../utils.h"
#include <cglm/types.h>
#include <stdint.h>

typedef struct {
  vec2 translate;
} Transform;

typedef struct {
  uint32_t width;
  uint32_t height;
  uint32_t *buffer;
} Surface;

typedef struct {
  int x, y;
} Point;

typedef struct {
  float r;
  float g;
  float b;
  float a;
} ColorF ;

uint32_t pack_color(ColorF color);
void set_pixel(Surface *surface, uint32_t x, uint32_t y, uint32_t color);
void draw_gradient(Surface *surface);
void draw_span(Surface *surface, int y, int x0, int x1, uint32_t color);
void set_clear_color(Surface *surface, ColorF color);
void clear_surface(Surface *surface);
// void clear_color(Surface *surface, ColorF color);
void draw_filled_triangle(Surface *surface, Point p0, Point p1, Point p2, uint32_t color);
void plot_line(Surface *surface, int x0, int y0, int x1, int y1, ColorF color);
void draw_thick_line(Surface *surface, Point p0, Point p1, int thickness, ColorF color);

