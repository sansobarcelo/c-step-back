#include "draw_context.h"
#include "graphics/drawer.h"
#include <stdio.h>

void draw_context_draw_thick_line(DrawContext *ctx, vec2 start, vec2 end, float thickness, ColorF color) {
  vec2 screen_start, screen_end;
  canvas_transform_point(&ctx->canvas, start, screen_start);
  canvas_transform_point(&ctx->canvas, end, screen_end);
  printf("Original: %f.2, %f.2\n", start[0], start[1]);
  printf("Updated: %f.2, %f.2\n", screen_start[0], screen_start[1]);
  draw_thick_line(&ctx->surface, (Point){.x = screen_start[0], .y = screen_start[1]}, (Point){.x = screen_end[0], .y = screen_end[1]}, 1.0, color);
}
