#ifndef CANVAS_H
#define CANVAS_H

#include <cglm/cglm.h>
#include <stdint.h>

#define CANVAS_STACK_MAX 16

typedef struct Canvas {
  mat3 transform;
  mat3 stack[CANVAS_STACK_MAX];
  int stack_top;

  float width;
  float height;

  float scale;
  vec2 position;
} Canvas;

void canvas_init(Canvas *canvas, float screen_width, float screen_height);
void canvas_translate(Canvas *canvas, float tx, float ty);
void canvas_scale(Canvas *canvas, float scale);
void canvas_rotate(Canvas *canvas, float radians);
void canvas_update_transform(Canvas *canvas);
void canvas_world_to_screen(Canvas *canvas, vec2 world, vec2 screen);
void canvas_screen_to_world(Canvas *canvas, vec2 screen, vec2 world);
void canvas_transform_point(Canvas *canvas, vec2 world, vec2 screen);

#endif // CANVAS_H
