#ifndef COMPONENTS_INPUT_H
#define COMPONENTS_INPUT_H

#include "flecs.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_KEYS 512 // SDL supports 0–511
#define MAX_MOUSE_BUTTONS 8

typedef struct {
  bool down;     // currently held down
  bool pressed;  // went down this frame
  bool released; // went up this frame
} KeyState;

typedef struct {
  KeyState keys[MAX_KEYS];
  KeyState mouse_buttons[MAX_MOUSE_BUTTONS];

  // Mouse state
  uint32_t mouse_x, mouse_y;
  uint32_t mouse_delta_x, mouse_delta_y;
  float wheel_x, wheel_y;

  // Modifiers
  bool shift, ctrl, alt;

  // Dragging logic
  bool mouse_dragging;
  bool mouse_was_dragging;
  uint32_t drag_origin_x, drag_origin_y;

  // Zoom request (used by systems)
  bool zoom_requested;
  float zoom_factor;
  uint32_t zoom_center_x;
  uint32_t zoom_center_y;
} Input;

void handle_input_system(ecs_iter_t *it) {
  Input *input = ecs_field(it, Input, 0); // Renderer($)
}

#endif
