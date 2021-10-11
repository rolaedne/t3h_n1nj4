#ifndef _WORLD_H
#define _WORLD_H

#ifdef __cplusplus
extern "C" {
#endif
/*******************************************
*Roderick Newill
*World.h
*Header for all of the world functions that
*are needed to build and maintain the world
********************************************/

#include <SDL2/SDL.h>
#include "utils.h"

#define MOVERL 10
#define SCREENWIDTH 640
#define SCREENHEIGHT 480
#define BRICK_HEIGHT 80
#define BRICK_WIDTH 60
#define GRAVITY 5
#define WORLD_ROWS 8
#define WORLD_COLS 50

int world[WORLD_ROWS][WORLD_COLS];

SDL_Surface *screen;
SDL_Surface *background;
SDL_Surface *foreground;
SDL_Surface *number, *wscore; // for score UI
SDL_Surface *blood1;
int worldnum;
int enemymax;

typedef struct {
    int x;
    int y;
    int w;
    int h;
    int max_x;
    int max_y;
} Viewport;

Viewport vp;

void graphics_load();
void graphics_free();
void spawn_snow_particles();
void spawn_blood_particles(const bbox target);
void blit_tiles_to_background();
void load_current_world_from_file();
void center_viewport_on_target(const int target_x, const int target_y);
int is_collision(const int x, const int y);
void draw_world_background(SDL_Surface *screen);

#ifdef __cplusplus
}
#endif

#endif /* _WORLD_H */
