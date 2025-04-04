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


SoftwareOpenGlRenderer renderer_create(uint32_t width, uint32_t height);
void renderer_free(SoftwareOpenGlRenderer *renderer);
void renderer_set_clear_color(SoftwareOpenGlRenderer *renderer, ColorF color);

// ECS
void render_system(ecs_iter_t *it);
void surface_resize_system(ecs_iter_t *it);

#endif
