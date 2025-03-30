#include "canvas.h"
#include <cglm/affine.h>
#include <math.h>

// PRIVATE: recompute matrix from state
void canvas_update_transform(Canvas *canvas) {
    glm_mat3_identity(canvas->transform);

    // Scale first
    glm_mat3_scale(canvas->transform, canvas->scale);

    // Then translate (in world space, affects the final result)
    mat3 translation;
    glm_mat3_identity(translation);
    translation[2][0] = -canvas->translation[0];
    translation[2][1] = -canvas->translation[1];

    glm_mat3_mul(canvas->transform, translation, canvas->transform);
}

// PUBLIC
void canvas_init(Canvas *canvas, float width, float height) {
    glm_mat3_identity(canvas->transform);
    canvas->stack_top = 0;
    canvas->width = width;
    canvas->height = height;
    canvas->scale = 1.0f;
    canvas->translation[0] = 0.0f;
    canvas->translation[1] = 0.0f;
    canvas_update_transform(canvas);
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
    canvas->translation[0] += tx;
    canvas->translation[1] += ty;
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

void canvas_transform_point(Canvas* canvas, vec2 world, vec2 screen) {
    // TODO: remove from here
        canvas_update_transform(canvas);

    vec3 input = {world[0], world[1], 1.0f};
    vec3 result;
    glm_mat3_mulv(canvas->transform, input, result);

    screen[0] = result[0] + canvas->width / 2.0f;
    screen[1] = canvas->height / 2.0f - result[1]; // Flip Y to match SDL top-left origin
}
