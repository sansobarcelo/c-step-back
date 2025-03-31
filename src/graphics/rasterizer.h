#pragma once

#include "../utils.h"
#include <cglm/types.h>
#include <stdint.h>

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

void rasterizer_set_clear_color(Surface *surface, ColorF color);
void rasterizer_clear_surface(Surface *surface);
void rasterizer_draw_thick_line(Surface *surface, Point p0, Point p1, int thickness, ColorF color);

