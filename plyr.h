#ifndef _PLYR_H
#define _PLYR_H

#ifdef __cplusplus
extern "C" {
#endif

// set up verlocity on jump
#define JUMPMAX -47
#define ATTLEN 20

typedef struct {
    int deaths;
    int score;
    int gravity_compound;
    int attack;
    int sattack;
    int sattack_length;
    int is_facing_right;
    int is_jumping;
    int is_attacking;
    int is_running;
    int is_dead;
    int x;
    int y;
    int w;
    int h;
    SDL_Rect spdest;
    SDL_Surface *ninja;
    SDL_Surface *sweapon1_1, *sweapon1_2;
} Player;

Player player;

void player_physics();
void special();
void draw_player();

#ifdef __cplusplus
}
#endif

#endif /* _PLYR_H */

