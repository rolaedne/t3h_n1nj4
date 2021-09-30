#ifndef _PLYR_H
#define _PLYR_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************
*Roderick Newill
*plr.h
*Header file for all of the ninja's functions
********************************************/

#define JUMPMAX -47
/*set up verlocity on jump*/
#define ATTLEN 20


// ninja_src is references a 360w x 240h (6 x 3) sprite sheet
// the ninja is 60w x 80h
// the left half is left facing, the right half is right facing
// the top row is jumping (nothing, sword out (left), sword sheathed (left), sword sheathed (right), sword out (right), nothing)
// the middle row is "neutral" (run-left-1, run-left-2, standing-left, standing-right, run-right-2, run-right-1)
// the bottom row is sword-drawn (run-left-1, run-left-2, standing-left, standing-right, run-right-2, run-right-1)

typedef struct {
    int is_dead;
    int deaths;
    int score;
    int gravity_compound;
    int jump;
    int attack;
    int sattack;
    int attacklen;
    SDL_Rect ninja_src;
    SDL_Rect dest;
    SDL_Rect spdest;
    SDL_Surface *ninja;
    SDL_Surface *sweapon1_1, *sweapon1_2;
} Player;

Player player;


void jumping(int jump);
void physics();
void killenemy();
void special();
void special_throw();


#ifdef __cplusplus
}
#endif

#endif /* _PLYR_H */

