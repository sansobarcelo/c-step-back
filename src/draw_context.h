#ifndef DRAW_CONTEXT_H
#define DRAW_CONTEXT_H 

#include "canvas.h"
#include "graphics/drawer.h"

typedef struct DrawContext {
    Canvas canvas;
    Surface surface;
} DrawContext;

void draw_context_draw_thick_line(DrawContext *ctx, vec2 start, vec2 end, float thickness, ColorF color);
#endif
