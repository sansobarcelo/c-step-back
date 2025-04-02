#ifndef COMPONENTS_INPUT_H
#define COMPONENTS_INPUT_H

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

  // Mouse position
  int mouse_x;
  int mouse_y;
  int mouse_delta_x;
  int mouse_delta_y;

  // Mouse wheel
  float wheel_x;
  float wheel_y;

  // Modifier keys
  bool shift;
  bool ctrl;
  bool alt;
} Input;

#endif // COMPONENTS_INPUT_H
