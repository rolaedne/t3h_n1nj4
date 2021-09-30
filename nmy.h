#ifndef _NMY_H
#define _NMY_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************
*Roderick Newill
*nmy.h
*This holds the function that the enemies will
*use to move, kill, and live.
********************************************/

#define NMYDLY 15
#define BYSWORD 0
#define BYSTAR  1
#define BYLAVA  2
#define NMY_FRAMES 6
#define NMY_DEATHS 3
#define NMY 10

/******************************************
 * defines all parts of the enemies
 * can we ever really define what or whom an enemy is
 * what qualities really make up an enemy
*******************************************/
typedef struct {
    SDL_Surface *anim_frames[NMY_FRAMES]; // frames of animation
    SDL_Surface *anim_frames_flipped[NMY_FRAMES]; // frames of animation, flipped horizontally
    SDL_Surface *death_frames[NMY_DEATHS]; // death frames
    SDL_Surface *death_frames_flipped[NMY_DEATHS]; // death frames, flipped horizontally
    SDL_Rect dest; // Abused to track enemy location
    int death_type;
    int death_bleed_counter;
    int is_alive;
    int is_visible; /*if enemy is on screen*/
    int is_flipped;
    int gravity_compound;
    int type; /*type of enemy*/
    int anim_delay;/*a count var NMYDLY in nmy.c tells lenght of delay*/
    int anim_frame;/*what animation to use*/
    /*********************
    * keeps track of last hit object in world. used mainly for ai to handle direction and jumping.
    * 5 would be in that air everything else would be around enemy.
    * 789
    * 456
    * 123
    *******************/
    int dir[10]; //see above
    int jump_is_active; // are they jumping
    int jump_strength; // how high enemies jump
    int speed; // how fast an enemy moves
} enemy;

enemy enemies[NMY];

void check_player_collision(enemy *e);
void enemy_physics(enemy *e);
void enemy_ai();

#ifdef __cplusplus
}
#endif

#endif /* _NMY_H */

