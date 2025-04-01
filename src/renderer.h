#include "components.h"
#include "drawer.h"
#include "SDL3/SDL_opengl.h"

typedef struct {
  GLuint texture;
  DrawContext draw_context;
} SoftwareOpenGlRenderer;

SoftwareOpenGlRenderer renderer_create(uint32_t width, uint32_t height);
// void renderer_render(SoftwareOpenGlRenderer *renderer);
void renderer_render(SoftwareOpenGlRenderer *renderer, Line *line, int count);
void renderer_handle_resize(SoftwareOpenGlRenderer *renderer, uint32_t new_width, uint32_t new_height);
void renderer_free(SoftwareOpenGlRenderer *renderer);
void renderer_set_clear_color(SoftwareOpenGlRenderer *renderer, ColorF color);
