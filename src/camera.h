#include "cglm/types.h"
#include <stdint.h>

typedef struct {
  vec2 position;
  double zoom;
} Camera;

typedef struct {
    float left;
    float right;
    float top;
    float bottom;
} Bounds;

void compute_view_matrix(mat3 view, Camera *camera);
void compute_projection_matrix(mat3 proj, float screen_width, float screen_height);
void world_to_screen(vec2 screen_pos, vec2 world_pos, Camera *camera, float screen_width, float screen_height);
Bounds camera_visible_bounds(const Camera *camera, uint32_t screen_width, uint32_t screen_height);
