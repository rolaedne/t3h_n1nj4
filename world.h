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

#include <SDL/SDL.h>

#define MOVERL 10
#define SCREENWIDTH 640
#define SCREENHEIGHT 480
#define BRICK_HEIGHT 80
#define BRICK_WIDTH 60
#define GRAVITY 5
#define WORLD_ROWS 6
#define WORLD_COLS 50

int world[WORLD_ROWS][WORLD_COLS];

SDL_Surface *screen;
SDL_Surface *background;
SDL_Surface *foreground;
SDL_Surface *number, *wscore; // for score UI
SDL_Rect wrldps;
int worldnum;
int enemymax;

void graphics_load();
void graphics_free();
void spawn_snow_particles();
void spawn_blood_particles(SDL_Rect bleed);
void set_screen();
void load_current_world_from_file();
void world_mover();
int is_collision(const int x, const int y);

#ifdef __cplusplus
}
#endif

#endif /* _WORLD_H */
