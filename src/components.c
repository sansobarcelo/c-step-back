#include "components.h"

void transform_points(Transform *transform, vec2 *in_points, vec2 *out_points, int count) {
  mat3 world;
  for (int i = 0; i < count; i++) {
    // printf("IN %.2f, %.2f\n", in_points[i][0], in_points[i][1]);
    glm_vec2_add(transform->position, in_points[i], out_points[i]);
    // printf("OUT %.2f, %.2f\n", out_points[i][0], out_points[i][1]);
  }
}
