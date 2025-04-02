#include "components.h"

void transform_points(Position *position, vec2 *in_points, vec2 *out_points, int count) {

  mat3 world;
  for (int i = 0; i < count; i++) {
    // printf("IN %.2f, %.2f\n", in_points[i][0], in_points[i][1]);
    vec2 pos = {position->pos_x, position->pos_y};
    glm_vec2_add(pos, in_points[i], out_points[i]);
    // printf("OUT %.2f, %.2f\n", out_points[i][0], out_points[i][1]);
  }
}
