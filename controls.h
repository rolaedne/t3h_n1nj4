#ifndef CONTROLS_H
#define CONTROLS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <SDL2/SDL.h>

typedef struct {
    SDL_Scancode jump;
    SDL_Scancode up;
    SDL_Scancode down;
    SDL_Scancode left;
    SDL_Scancode right;
    SDL_Scancode attack;
    SDL_Scancode special_attack;
    SDL_Scancode debug_skip_level;
    SDL_Scancode debug_pause_enemies;
    SDL_Scancode debug_show_bounding_boxes;
} Controls;

Controls controls;

void init_controls();

#ifdef __cplusplus
}
#endif

#endif /* CONTROLS_H */