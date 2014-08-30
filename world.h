#ifndef _WORLD_H
#define	_WORLD_H

#ifdef	__cplusplus
extern "C" {
#endif
/*******************************************
*Roderick Newill
*World.h
*Header for all of the world functions that
*are needed to build and maintain the world
********************************************/

#include <SDL/SDL.h>

#define BRICK_HEIGHT 80
#define BRICK_WIDTH 60
#define SCREENWIDTH 640
#define SCREENHEIGHT 480
#define MOVERL 10
#define GRAVITY 5
#define NMY 10
#define WORLD_ROWS 6
#define WORLD_COLS 50
#define BRICKS_WORLD 5
#define BRICKS_DAMAGE 5

SDL_Surface *dmgbrick[BRICKS_DAMAGE];
int world_length;
int worldnum;
SDL_Surface *screen, *background, *worldfloor, *worldbrick[BRICKS_WORLD];
SDL_Surface *number, *wscore;
SDL_Rect worlddest;
SDL_Event event;
SDL_Rect wrldps;
int world[WORLD_ROWS][WORLD_COLS];
int enemymax;

void graphics_load();
void graphics_free();
void rprint(int val);
void blood(SDL_Rect bleed);
void set_screen();
void buildw();
void world_mover();
int twoblock_col(int bb1x, int bb1y, int bb1w, int bb1h,
                  int bb2x, int bb2y, int bb2w, int bb2h);



#ifdef	__cplusplus
}
#endif

#endif	/* _WORLD_H */

