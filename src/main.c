#include "flecs.h"
#include "input.h"
#include "renderer.h"
#include <stdio.h>
#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include "sdl_app.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

ECS_COMPONENT_DECLARE(Input);

void Move(ecs_iter_t *it) {}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  printf("SDL App Init\n");
  ecs_world_t *world = ecs_init_w_args(argc, argv);
  *appstate = world; // SDL app state will be a pointer to the world

  // Order matters!

  // Register components
  ECS_COMPONENT_DEFINE(world, Input);

  // Register singletons
  ecs_set(world, ecs_id(Input), Input, {0});

  // Manual system
  // ECS_SYSTEM(world, renderer_resize_system, EcsOnUpdate);

  // Register systems
  ECS_SYSTEM(world, handle_input_system, EcsOnUpdate, Input($));
  ECS_SYSTEM(world, Move, EcsOnUpdate);

  // Register modules
  ECS_IMPORT(world, Sdl);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  // printf("APP: SDL event\n");
  ecs_world_t *world = appstate;

  // Get mutable input singleton
  Input *input = ecs_get_mut(world, ecs_id(Input), Input);

  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS; /* end the program, reporting success to the OS. */
  } else if (event->type == SDL_EVENT_WINDOW_RESIZED) {
    // ecs_run(world, ecs_id(renderer_resize_system), 0.0 /* delta_time */, NULL /* param */);
  }

  // Imgui event
  // custom event

  return SDL_APP_CONTINUE; /* carry on with the program! */
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  // printf("SDL iterate\n");
  ecs_world_t *world = appstate;
  const App *app = ecs_singleton_get(world, App);
  if (app->status != SDL_APP_CONTINUE) {
    return app->status;
  }

  ecs_progress(world, 0);

  app = ecs_singleton_get(world, App);
  return app->status;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  ecs_world_t *world = appstate;
  ecs_fini(world);
}
