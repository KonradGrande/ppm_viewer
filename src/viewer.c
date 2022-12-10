#include "viewer.h"
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>

#define CheckReturnValue(check)                                                \
  if (check) {                                                                 \
    fprintf(stderr, "SDL ERROR: %s\n", SDL_GetError());                        \
    exit(EXIT_FAILURE);                                                        \
  }

void sdl_cc(int code) { CheckReturnValue(code < 0) }

void *sdl_cp(void *ptr) {
  CheckReturnValue(ptr == NULL);
  return ptr;
}

viewer_t *viewer_init(char *filename) {
  viewer_t *viewer = malloc(sizeof(viewer_t));
  if (viewer == NULL) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  bool fullscreen = false;

  sdl_cc(SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS) == 0);

  int flags = SDL_WINDOW_RESIZABLE;
  if (fullscreen)
    flags |= SDL_WINDOW_FULLSCREEN;
  viewer->window = (SDL_Window *)sdl_cp(
      SDL_CreateWindow(filename, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       500, 500, flags));

  viewer->renderer = (SDL_Renderer *)sdl_cp(
      SDL_CreateRenderer(viewer->window, -1, SDL_RENDERER_ACCELERATED));

  viewer->is_running = true;

  viewer_load_image(viewer, filename);

  return viewer;
}

void viewer_load_image(viewer_t *viewer, char *filename) {
  viewer_free_image(viewer);
  viewer->image = image_read(filename);
  if (viewer->image == NULL) {
    fprintf(stderr, "failed to parse '%s' as a ppm file.\n", filename);
    exit(EXIT_FAILURE);
  }
  viewer->filename = filename;

  viewer->pixels = malloc(sizeof(SDL_Rect) * viewer->image->num_pixels);
  if (viewer->pixels == NULL) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }
}

void viewer_reload_image(viewer_t *viewer) {
  viewer_load_image(viewer, viewer->filename);
}

void viewer_compute_pixel_positions(viewer_t *viewer) {
  int height = viewer->image->height;
  int width = viewer->image->width;

  float size_height = viewer->height / (float)height;
  float size_width = viewer->width / (float)width;
  float size = size_height > size_width ? size_width : size_height;

  float offset_x = (viewer->width - size * width) / 2;
  float offset_y = (viewer->height - size * height) / 2;

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      SDL_FRect *p = &viewer->pixels[y * width + x];
      p->w = p->h = size;
      p->x = offset_x + x * size;
      p->y = offset_y + y * size;
    }
  }
}

void viewer_render_image(viewer_t *viewer) {
  color_t bg = color(0x282C34);
  SDL_SetRenderDrawColor(viewer->renderer, bg.r, bg.g, bg.b, 0xff);
  SDL_RenderFillRect(viewer->renderer, NULL);

  for (int i = 0; i < viewer->image->num_pixels; i++) {
    color_t color = viewer->image->pixels[i];
    SDL_SetRenderDrawColor(viewer->renderer, color.r, color.g, color.b, 0xff);
    SDL_RenderFillRectF(viewer->renderer, &viewer->pixels[i]);
  }

  SDL_RenderPresent(viewer->renderer);
}

void viewer_free_image(viewer_t *viewer) {
  if (viewer->image != NULL)
    image_free(viewer->image);

  if (viewer->pixels != NULL)
    free(viewer->pixels);
}

void viewer_clean(viewer_t *viewer) {
  image_free(viewer->image);
  free(viewer->pixels);
  SDL_DestroyWindow(viewer->window);
  SDL_DestroyRenderer(viewer->renderer);
  free(viewer);
  SDL_Quit();
}

void viewer_handle_events(viewer_t *viewer) {
  SDL_Event event;
  if (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      viewer->is_running = false;
      break;
    case SDL_WINDOWEVENT:
      switch (event.window.event) {
      case SDL_WINDOWEVENT_RESIZED:
        viewer->width = event.window.data1;
        viewer->height = event.window.data2;
        SDL_SetWindowSize(viewer->window, viewer->width, viewer->height);
        viewer_compute_pixel_positions(viewer);
        viewer_render_image(viewer);
        break;
      case SDL_WINDOWEVENT_EXPOSED:
        viewer_render_image(viewer);
        break;
      default:
        break;
      }
    default:
      break;
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "usage: viewer <filename>\n");
    exit(EXIT_FAILURE);
  }

  char *filename = argv[1];
  printf("viewing '%s'\n", filename);

  viewer_t *viewer = viewer_init(filename);
  viewer_render_image(viewer);

  while (viewer->is_running) {
    viewer_handle_events(viewer);
    SDL_Delay(30);
  }

  viewer_clean(viewer);

  return 0;
}
