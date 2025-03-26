#include "graphics/drawer.h"
#include "SDL3/SDL_opengl.h"
#include "camera.h"
#include <stdint.h>

typedef struct {
  Surface surface;
  GLuint texture;
} SoftwareOpenGlRenderer ;

SoftwareOpenGlRenderer renderer_create(uint32_t width, uint32_t height);
void renderer_render(SoftwareOpenGlRenderer *renderer, ColorF color, Camera *camera);
void renderer_handle_resize(SoftwareOpenGlRenderer *renderer, uint32_t new_width, uint32_t new_height);
void renderer_free(SoftwareOpenGlRenderer *renderer);
