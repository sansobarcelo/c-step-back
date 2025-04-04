
#include "renderer.h"

#include "SDL3/SDL_opengl.h"
#include "canvas.h"
#include "cglm/types.h"
#include "components.h"
#include "drawer.h"
#include "graphics/rasterizer.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Private
void update_texture(GLuint texture, const Surface *surface) {
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, surface->width, surface->height, GL_RGBA, GL_UNSIGNED_BYTE, surface->buffer);
}

Surface create_surface(uint32_t width, uint32_t height) {
  return (Surface){
      .width = width,
      .height = height,
      .buffer = malloc(width * height * sizeof(uint32_t)),
  };
}

GLuint create_texture_from_surface(const Surface *surface) {
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Prevent blurring
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA, surface->width, surface->height, 0, GL_BGRA, GL_UNSIGNED_BYTE, surface->buffer);
  return texture;
}
// End Private

// Public implementations
void renderer_resize_system(ecs_iter_t *it) {
  printf("Resized!\n");
  SoftwareOpenGlRenderer *renderer = ecs_field(it, SoftwareOpenGlRenderer, 0); // Renderer($)
  Canvas *canvas = &renderer->draw_context.canvas;
  Surface *surface = &renderer->draw_context.surface;

  // ResizeParams *params = it->param;
  // uint32_t new_width = params->width;
  // uint32_t new_height = params->height;
  //
  // free(surface->buffer);
  // glDeleteTextures(1, &renderer->texture);
  // renderer->draw_context.surface = create_surface(new_width, new_height);
  // renderer->texture = create_texture_from_surface(&renderer->draw_context.surface);
  // renderer->draw_context.canvas.height = new_height;
  // renderer->draw_context.canvas.width = new_width;
}

void render_system(ecs_iter_t *it) {
  SoftwareOpenGlRenderer *renderer = ecs_field(it, SoftwareOpenGlRenderer, 2); // Renderer($)

  Canvas *canvas = &renderer->draw_context.canvas;
  Surface *surface = &renderer->draw_context.surface;

  canvas_update_transform(canvas);
  rasterizer_clear_surface(surface);

  float thickness = 10 * canvas->scale;
  Line *line = ecs_field(it, Line, 0); // regular field
  Position *transform = ecs_field(it, Position, 1);

  for (int i = 0; i < it->count; i++) {
    ColorF color = {.r = 0.0f, .g = 0.0f, .b = 1.0f, .a = 1.0f};

    vec2 points[2];
    glm_vec2_copy(line[i].a, points[0]);
    glm_vec2_copy(line[i].b, points[1]);

    transform_points(&transform[i], points, points, 2);
    draw_context_draw_thick_line(&renderer->draw_context, points[0], points[1], thickness, color);
  }
}

void renderer_render(SoftwareOpenGlRenderer *renderer, ecs_world_t *world, ecs_query_t *query) {
  Surface *surface = &renderer->draw_context.surface;
  Canvas *canvas = &renderer->draw_context.canvas;

  printf("Hello from renderer, %.2f \n", canvas->position[0]);

  canvas_update_transform(canvas);
  rasterizer_clear_surface(surface);

  // TODO: Hard coded draw system for lines
  float thickness = 10 * canvas->scale;
  ecs_iter_t it = ecs_query_iter(world, query);
  while (ecs_query_next(&it)) {
    Line *line = ecs_field(&it, Line, 0);
    Position *transform = ecs_field(&it, Position, 1);

    for (int i = 0; i < it.count; i++) {
      ColorF color = {.r = 0.0f, .g = 0.0f, .b = 1.0f, .a = 1.0f};

      // Move to world
      vec2 points[2];
      points[0][0] = line[i].a[0];
      points[0][1] = line[i].a[1];
      points[1][0] = line[i].b[0];
      points[1][1] = line[i].b[1];

      transform_points(&transform[i], points, points, 2);
      draw_context_draw_thick_line(&renderer->draw_context, points[0], points[1], thickness, color);
    }
  }

  // Draw line
  ColorF color = {.r = 0.2f, .g = 0.4f, .b = 1.0f, .a = 1.0f};
  vec2 p0 = {0, 50};
  vec2 p1 = {200, 50};
  draw_context_draw_thick_line(&renderer->draw_context, p0, p1, thickness, color);

  update_texture(renderer->texture, surface);
}

SoftwareOpenGlRenderer renderer_create(uint32_t width, uint32_t height) {
  Surface surface = create_surface(width, height);
  GLuint texture = create_texture_from_surface(&surface);
  Canvas canvas;
  canvas_init(&canvas, width, height);

  DrawContext draw_context = {
      .surface = surface,
      .canvas = canvas,
  };
  return (SoftwareOpenGlRenderer){
      .draw_context = draw_context,
      .texture = texture,
  };
}

void renderer_free(SoftwareOpenGlRenderer *renderer) {
  Surface *surface = &renderer->draw_context.surface;
  glDeleteTextures(1, &renderer->texture);
  free(surface->buffer);
}

void renderer_handle_resize(SoftwareOpenGlRenderer *renderer, uint32_t new_width, uint32_t new_height) {
  Surface *surface = &renderer->draw_context.surface;
  free(surface->buffer);
  glDeleteTextures(1, &renderer->texture);
  renderer->draw_context.surface = create_surface(new_width, new_height);
  renderer->texture = create_texture_from_surface(&renderer->draw_context.surface);
  renderer->draw_context.canvas.height = new_height;
  renderer->draw_context.canvas.width = new_width;
}

void renderer_set_clear_color(SoftwareOpenGlRenderer *renderer, ColorF color) { rasterizer_set_clear_color(&renderer->draw_context.surface, color); }
