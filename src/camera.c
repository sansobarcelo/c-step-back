#include "camera.h"
#include "cglm/mat3.h"

void screen_to_world(vec2 out_world, float screen_x, float screen_y, const Camera *camera, float screen_width, float screen_height) {
  out_world[0] = (screen_x - screen_width / 2.0f) / camera->zoom + camera->position[0];
  out_world[1] = (screen_height / 2.0f - screen_y) / camera->zoom + camera->position[1];
}

Bounds camera_visible_bounds(const Camera *camera, uint32_t screen_width, uint32_t screen_height) {
  float half_width = (float)screen_width / (2.0f * camera->zoom);
  float half_height = (float)screen_height / (2.0f * camera->zoom);

  return (Bounds){
      .left = camera->position[0] - half_width,
      .right = camera->position[0] + half_width,
      .bottom = camera->position[1] - half_height,
      .top = camera->position[1] + half_height,
  };
}

void compute_view_matrix(mat3 view, Camera *camera) {

  glm_mat3_identity(view);

  /*
    The view matrix transforms world coordinates relative to the camera.
    It follows this general form:

    V =
    |  1    0    0  |   <-- No rotation (for now)
    |  0    1    0  |   <-- No rotation (for now)
    | -Tx  -Ty   1  |   <-- Translation to move world opposite to camera

    Where:
    - Tx = camera->position[0]
    - Ty = camera->position[1]

    This moves everything in the opposite direction of the camera’s position.
 */
  view[2][0] = -camera->position[0];
  view[2][1] = -camera->position[1];

  /*
         Next, apply zoom (scaling). We multiply each coordinate by `zoom`:

         S =
         | zoom   0   0  |
         |  0   zoom  0  |
         |  0     0   1  |

         This makes objects appear larger or smaller depending on `camera->zoom`.
      */

  glm_mat3_scale(view, camera->zoom);

  /*
         The final view matrix is:

         V' = S * V =
         | zoom       0       0  |
         |  0        zoom     0  |
         | -Tx*zoom  -Ty*zoom  1 |

         This means:
         - The world moves by `-camera->position * zoom`.
         - The zoom factor scales everything proportionally.
      */
}

void compute_projection_matrix(mat3 proj, float screen_width, float screen_height) {
  glm_mat3_identity(proj);

  // Map world coordinates to NDC (-1,1)
  // ie: Not needed for SDL

  proj[0][0] = 2.0f / screen_width;
  proj[1][1] = 2.0f / screen_height;
}

void world_to_screen(vec2 screen_pos, vec2 world_pos, Camera *camera, float screen_width, float screen_height) {
  mat3 view;
  compute_view_matrix(view, camera); // Compute the view matrix

  vec3 tmp_word_pos = {world_pos[0], world_pos[1], 1.0f};

  vec3 transformed;
  glm_mat3_mulv(view, tmp_word_pos, transformed);

  // Convert to screen space
  screen_pos[0] = transformed[0] + screen_width / 2.0f;
  screen_pos[1] = screen_height / 2.0f - transformed[1]; // Flip Y (SDL top-left)

  // Prevent precision errors causing wrap-around
  if (screen_pos[0] < -screen_width || screen_pos[0] > 2 * screen_width) {
    screen_pos[0] = -10000; // Move it off-screen
  }
  if (screen_pos[1] < -screen_height || screen_pos[1] > 2 * screen_height) {
    screen_pos[1] = -10000; // Move it off-screen
  }
}
