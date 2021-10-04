#ifndef CONTROLS_H
#define CONTROLS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <SDL2/SDL.h>
#include "utils.h"

typedef struct {
    int state;
    SDL_Scancode key;
} Stateful_Key;

typedef struct {
    SDL_Scancode jump;
    SDL_Scancode up;
    SDL_Scancode down;
    SDL_Scancode left;
    SDL_Scancode right;
    SDL_Scancode attack;
    Stateful_Key special_attack;
    Stateful_Key debug_skip_level;
    Stateful_Key debug_pause_enemies;
    Stateful_Key debug_show_bounding_boxes;
} Controls;

Controls controls;

typedef struct {
    Boolean attack;
    Boolean special_attack;
    Boolean jump;
    Boolean left;
    Boolean right;
    Boolean up;
    Boolean down;
    Boolean mouse_attack;
    Boolean mouse_special_attack;
    int mouse_x;
    int mouse_y;
    int mouse_button;
    Boolean debug_skip_level;
    Boolean debug_pause_enemies;
} Inputs;

void init_controls();
Inputs check_inputs();
int check_stateful_key_down(const Uint8* keys, Stateful_Key *stateful_key);

#ifdef __cplusplus
}
#endif

#endif /* CONTROLS_H */