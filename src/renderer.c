#include "renderer.h"
#include "SDL3/SDL_opengl.h"
#include <stdlib.h>

// Private
void update_texture(GLuint texture, const Surface *surface) {
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, surface->width, surface->height, GL_BGRA, GL_UNSIGNED_BYTE, surface->buffer);
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
void renderer_render(SoftwareOpenGlRenderer *renderer, Camera *camera) {
  Surface *surface = &renderer->surface;

  clear_color(surface, 100, 30, 10);

  // Object example
  vec2 p0 = {0, 0};
  vec2 p1 = {100, 0};
  world_to_screen(p0, p0, camera, surface->width, surface->height);
  world_to_screen(p1, p1, camera, surface->width, surface->height);
  draw_thick_line(surface, (Point){p0[0], p0[1]}, (Point){p1[0], p1[1]}, 25, 10, 244, 10);

  // Once drawn everything on buffer
  update_texture(renderer->texture, &renderer->surface);
}

SoftwareOpenGlRenderer renderer_create(uint32_t width, uint32_t height) {
  Surface surface = create_surface(width, height);
  GLuint texture = create_texture_from_surface(&surface);
  return (SoftwareOpenGlRenderer){
      .surface = surface,
      .texture = texture,
  };
}

void renderer_free(SoftwareOpenGlRenderer *renderer) {
  glDeleteTextures(1, &renderer->texture);
  free(renderer->surface.buffer);
}

void renderer_handle_resize(SoftwareOpenGlRenderer *renderer, uint32_t new_width, uint32_t new_height) {
  free(renderer->surface.buffer);
  glDeleteTextures(1, &renderer->texture);

  renderer->surface = create_surface(new_width, new_height);
  renderer->texture = create_texture_from_surface(&renderer->surface);
}
