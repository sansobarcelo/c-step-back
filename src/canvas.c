#include "canvas.h"
#include "cglm/mat3.h"
#include <cglm/affine.h>

// PRIVATE: recompute matrix from state
void canvas_update_transform(Canvas *canvas) {
  mat3 scale_mat, translate_mat;

  // Create identity matrices
  glm_mat3_identity(scale_mat);
  glm_mat3_identity(translate_mat);

  // Set up scale
  scale_mat[0][0] = canvas->scale;
  scale_mat[1][1] = canvas->scale;

  // Set up position
  // Negate if camera move like, if canvas is right positioned, the objects should display on left.
  // Same vertical
  translate_mat[2][0] = -canvas->position[0];
  translate_mat[2][1] = -canvas->position[1];

  // Final transform = translate * scale
  glm_mat3_mul(translate_mat, scale_mat, canvas->transform);
}

// PUBLIC
void canvas_init(Canvas *canvas, float width, float height) {
  glm_mat3_identity(canvas->transform);
  canvas->stack_top = 0;
  canvas->width = width;
  canvas->height = height;
  canvas->scale = 1.0f;
  canvas->position[0] = 0.0f;
  canvas->position[1] = 0.0f;
  canvas_update_transform(canvas); // Ensure initial transform is set
}

void canvas_save(Canvas *canvas) {
  if (canvas->stack_top < CANVAS_STACK_MAX) {
    glm_mat3_copy(canvas->transform, canvas->stack[canvas->stack_top]);
    canvas->stack_top++;
  }
}

void canvas_restore(Canvas *canvas) {
  if (canvas->stack_top > 0) {
    canvas->stack_top--;
    glm_mat3_copy(canvas->stack[canvas->stack_top], canvas->transform);
  }
}

void canvas_translate(Canvas *canvas, float tx, float ty) {
  canvas->position[0] += tx;
  canvas->position[1] += ty;
  canvas_update_transform(canvas);
}

void canvas_scale(Canvas *canvas, float scale) {
  canvas->scale = scale;
  canvas_update_transform(canvas);
}

void canvas_rotate(Canvas *canvas, float radians) {
  // Optional: if you want rotation support later
  // Not used in update_transform, so would need to be added
}

void canvas_transform_point(Canvas *canvas, vec2 world, vec2 screen) {
  vec3 input = {world[0], world[1], 1.0f};
  vec3 result;
  glm_mat3_mulv(canvas->transform, input, result);

  screen[0] = result[0] + canvas->width / 2.0f;
  screen[1] = canvas->height / 2.0f + result[1];
}

void canvas_world_to_screen(Canvas *canvas, vec2 world, vec2 screen) { canvas_transform_point(canvas, world, screen); }

void canvas_screen_to_world(Canvas *canvas, vec2 screen, vec2 world) {
  float x = screen[0] - canvas->width / 2.0f;
  float y = screen[1] - canvas->height / 2.0f;

  mat3 inv;
  glm_mat3_inv(canvas->transform, inv);

  vec3 input = {x, y, 1.0f};
  vec3 result;
  glm_mat3_mulv(inv, input, result);

  world[0] = result[0];
  world[1] = result[1];
}
