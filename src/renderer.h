#ifndef RENDERER_H
#define RENDERER_H

#include "SDL3/SDL_opengl.h"
#include "drawer.h"
#include "flecs.h"
#include <stdint.h>

typedef struct {
  GLuint texture;
  DrawContext draw_context;
} SoftwareOpenGlRenderer;

typedef struct {
  uint32_t height;
  uint32_t width;
} ResizeParams;

SoftwareOpenGlRenderer renderer_create(uint32_t width, uint32_t height);
// // void renderer_render(SoftwareOpenGlRenderer *renderer);
// void renderer_render(SoftwareOpenGlRenderer *renderer, ecs_world_t *world, ecs_query_t *query);
// void renderer_handle_resize(SoftwareOpenGlRenderer *renderer, uint32_t new_width, uint32_t new_height);
void renderer_free(SoftwareOpenGlRenderer *renderer);
void renderer_set_clear_color(SoftwareOpenGlRenderer *renderer, ColorF color);

void render_system(ecs_iter_t *it);
void renderer_resize_system(ecs_iter_t *it);

#endif
