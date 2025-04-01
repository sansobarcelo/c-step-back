#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "cglm/types.h"
#include <cglm/cglm.h>

typedef struct {
  vec2 position;
} Transform;

typedef struct {
  vec2 a;
  vec2 b;
  Transform transform; // World info
  float thickness;
} Line;

void transform_points(Transform *transform, vec2 *in_points, vec2 *out_points, int count);
#endif // COMPONENTS_H
