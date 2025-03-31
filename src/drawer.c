#include "drawer.h"
#include <stdio.h>

void draw_context_draw_thick_line(DrawContext *ctx, vec2 start, vec2 end, float thickness, ColorF color) {
  Surface *surface = &ctx->surface;
  vec2 screen_start, screen_end;
  canvas_transform_point(&ctx->canvas, start, screen_start);
  canvas_transform_point(&ctx->canvas, end, screen_end);

  printf("Original: %.2f --> %.2f\n", end[1], screen_end[1]);
  // Removed Y flip
  // screen_start[1] = surface->height - screen_start[1] - 1;
  // screen_end[1] = surface->height - screen_end[1] - 1;

  thickness *= ctx->canvas.scale;

  Point a = {.x = screen_start[0], .y = screen_start[1]};
  Point b = {.x = screen_end[0], .y = screen_end[1]};

  rasterizer_draw_thick_line(surface, a, b, thickness, color);
}
