#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "cglm/types.h"
#include <cglm/cglm.h>

typedef struct {
  vec2 pos;
} Position;

typedef struct {
  vec2 a;
  vec2 b;
  float thickness;
} Line;

typedef struct {} Selected ;

void transform_points(Position *position, vec2 *in_points, vec2 *out_points, int count);
#endif // COMPONENTS_H
