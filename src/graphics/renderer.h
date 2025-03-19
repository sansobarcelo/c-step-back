#pragma once

#include "../stack.h"
#include "../utils.h"
#include <cglm/types.h>
#include <stdint.h>

typedef struct {
  vec2 translate;
} Transform;

STACK_DEFINE(Transform) // Define the stack for integers

typedef struct {
  Stack_Transform stack;
} Scene;

typedef struct {
  uint32_t width;
  uint32_t height;
  uint32_t *buffer;
} Surface;

typedef struct {
  int x, y;
} Point;

Scene scene_create() {
  Scene scene;
  Stack_Transform_init(&scene.stack, 10);
  return scene;
}

uint32_t pack_color(uint8_t r, uint8_t g, uint8_t b);
void set_pixel(Surface *surface, uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b);
void draw_gradient(Surface *surface);
void clear_color(Surface *surface, uint8_t r, uint8_t g, uint8_t b);
void plot_line(Surface *surface, int x0, int y0, int x1, int y1, int r, int g, int b);
void draw_span(Surface *surface, int y, int x0, int x1, uint32_t color);
void draw_filled_triangle(Surface *surface, Point p0, Point p1, Point p2, uint32_t color);
void draw_thick_line(Surface *surface, Point p0, Point p1, int thickness, uint8_t r, uint8_t g, uint8_t b);
