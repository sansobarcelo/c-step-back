#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "cglm/types.h"
#include <cglm/cglm.h>

typedef struct {
  float pos_x, pos_y;
} Position;

typedef struct {
  float ax, ay;
  float bx, by;
  float thickness;
} Line;

void transform_points(Position *position, vec2 *in_points, vec2 *out_points, int count);
#endif // COMPONENTS_H
