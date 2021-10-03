#include "controls.h"

void init_controls() {
    // classic control scheme
    controls.left = SDL_SCANCODE_LEFT;
    controls.right = SDL_SCANCODE_RIGHT;
    controls.jump = SDL_SCANCODE_UP;
    controls.attack = SDL_SCANCODE_SPACE;
    controls.special_attack = SDL_SCANCODE_DOWN;
    controls.debug_skip_level = SDL_SCANCODE_TAB;

    // Unused for now, but just wait!
    controls.down = SDL_SCANCODE_DOWN;
    controls.up = SDL_SCANCODE_UP;
    controls.debug_pause_enemies = SDL_SCANCODE_0;
    controls.debug_show_bounding_boxes = SDL_SCANCODE_9;
}