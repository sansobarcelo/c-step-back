#include "sdl_app.h"
#include <stdio.h>

ECS_COMPONENT_DECLARE(App);

void sdl_startup(ecs_iter_t *it) {
  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;

  App *app = (App *)ecs_field(it, App, 0);

  SDL_SetAppMetadata("flecs-sdl3-blend2d", "1.0", "com.example.flecs-sdl3-blend2d");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    app->status = SDL_APP_FAILURE;
    return;
  }

  if (!SDL_CreateWindowAndRenderer("flecs-sdl3-blend2d", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    app->status = SDL_APP_FAILURE;
    return;
  }

  ecs_singleton_set(it->world, App, {window, renderer, SDL_APP_CONTINUE});
}

void sdl_render_start(ecs_iter_t *it) {
  // printf("SDL render start\n");
  App *app = (App *)ecs_field(it, App, 0);
  SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(app->renderer);
}

void sdl_render_end(ecs_iter_t *it) {
  // printf("SDL render end\n");
  App *app = (App *)ecs_field(it, App, 0);
  SDL_RenderPresent(app->renderer);
}

void SdlImport(ecs_world_t *world) {
  // printf("SDL Import module into world\n");
  ECS_MODULE(world, Sdl);

  // Register components
  ECS_COMPONENT_DEFINE(world, App);

  // Register singletons
  ecs_singleton_set(world, App, {.status = SDL_APP_CONTINUE});

  // Register systems
  ECS_SYSTEM(world, sdl_startup, EcsOnStart, App($));
  ECS_SYSTEM(world, sdl_render_start, EcsOnUpdate, App($));
  ECS_SYSTEM(world, sdl_render_end, EcsPostFrame, App($));
}
