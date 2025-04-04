#include "input.h"
#include "flecs.h"
#include "flecs/addons/flecs_c.h"

ECS_COMPONENT_DECLARE(ResizeParams);
void input_register_components(ecs_world_t *world) { ECS_COMPONENT_DEFINE(world, ResizeParams); }
