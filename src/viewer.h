#ifndef VIEWER_H_
#define VIEWER_H_

#include "ppm/ppm.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

typedef struct viewer {
  SDL_Window *window;
  SDL_Renderer *renderer;
  bool is_running;
  char *filename;
  image_t *image;
  SDL_FRect *pixels;
  int height;
  int width;
} viewer_t;

void viewer_load_image(viewer_t *viewer, char *filename);
void viewer_reload_image(viewer_t *viewer);
void viewer_compute_pixel_positions(viewer_t *viewer);
void viewer_render_image(viewer_t *viewer);
void viewer_free_image(viewer_t *viewer);
void viewer_clean(viewer_t *viewer);
void viewer_handle_events(viewer_t *viewer);

#endif // VIEWER_H_
