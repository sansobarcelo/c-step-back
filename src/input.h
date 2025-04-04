#ifndef COMPONENTS_INPUT_H
#define COMPONENTS_INPUT_H

#include "flecs.h"
#include <stdbool.h>
#include <stdint.h>

#define MAX_KEYS 512 // SDL supports 0–511
#define MAX_MOUSE_BUTTONS 8

typedef struct {
  uint32_t height;
  uint32_t width;
} ResizeParams;

extern ECS_COMPONENT_DECLARE(ResizeParams);

void input_register_components(ecs_world_t *world);
#endif
