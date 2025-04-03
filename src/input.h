#ifndef COMPONENTS_INPUT_H
#define COMPONENTS_INPUT_H

#include "flecs.h"
#include <stdbool.h>
#include <stdint.h>

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
  int mouse_x, mouse_y;
  int mouse_delta_x, mouse_delta_y;
  float wheel_x, wheel_y;

  // Modifiers
  bool shift, ctrl, alt;

  // Dragging logic
  bool mouse_dragging;
  bool mouse_was_dragging;
  int drag_origin_x, drag_origin_y;

  // Zoom request (used by systems)
  bool zoom_requested;
  float zoom_factor;
  int zoom_center_x;
  int zoom_center_y;
} Input;

void input_updated(ecs_iter_t *it) {
  Input *input = (Input *)ecs_field(it, Input, 0);
  // update
  input->alt = true;
}

#endif
