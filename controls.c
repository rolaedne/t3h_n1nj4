#include "controls.h"
#include "utils.h"


void set_stateful_key(const Stateful_Key src, Stateful_Key *dest) {
    dest->key = src.key;
    dest->state = src.state;
}

void init_controls() {
    // arrow aim controls
    controls.left = SDL_SCANCODE_A;
    controls.right = SDL_SCANCODE_D;
    controls.up = SDL_SCANCODE_W;
    controls.down = SDL_SCANCODE_S;
    controls.jump = SDL_SCANCODE_W;
    controls.attack = SDL_SCANCODE_SPACE;
    set_stateful_key((Stateful_Key){ .key = SDL_SCANCODE_S,   .state = 0 }, &controls.special_attack);
    set_stateful_key((Stateful_Key){ .key = SDL_SCANCODE_TAB, .state = 0 }, &controls.debug_skip_level);
    set_stateful_key((Stateful_Key){ .key = SDL_SCANCODE_P,   .state = 0 }, &controls.debug_pause_enemies);
    set_stateful_key((Stateful_Key){ .key = SDL_SCANCODE_B,   .state = 0 }, &controls.debug_show_bounding_boxes);
}

int check_stateful_key_down(const Uint8* keys, Stateful_Key *stateful_key) {
    if (keys[stateful_key->key] && stateful_key->state == 0) {
        stateful_key->state = 1;
        return TRUE;
    }
    if (stateful_key->state == 1 && !keys[stateful_key->key]) {
        stateful_key->state = 0;
    }
    return FALSE;
}

Inputs check_inputs() {
    Inputs inputs = { .mouse_x = -1, .mouse_y = -1, .mouse_button = -1 };
    SDL_Event event;
    //printf("Checking events\n");
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (!inputs.jump && keys[controls.jump]) {
        inputs.jump = TRUE;
    }

    if (keys[controls.left] ^ keys[controls.right]) { // XOR, don't move left and right at the same time
        inputs.left = keys[controls.left];
        inputs.right = keys[controls.right];
    }
    if (keys[controls.up] ^ keys[controls.down]) { // XOR, don't look up and down at the same time
        inputs.up = keys[controls.up];
        inputs.down = keys[controls.down];
    }

    if (check_stateful_key_down(keys, &controls.special_attack)) {
        inputs.special_attack = TRUE;
    }

    if (keys[controls.attack]) {
        inputs.attack = TRUE;
    }

    if (check_stateful_key_down(keys, & controls.debug_skip_level)) {
        inputs.debug_skip_level = TRUE;
    }

    if (check_stateful_key_down(keys, &controls.debug_pause_enemies)) {
        inputs.debug_pause_enemies = TRUE;
    }

    if (check_stateful_key_down(keys, &controls.debug_show_bounding_boxes)) {
        inputs.debug_show_bounding_boxes = TRUE;
    }

    while (SDL_PollEvent(&event)) {//main even loop
        if (event.type == SDL_QUIT) {
            exit(0);
        }

        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                inputs.mouse_special_attack = TRUE;
                inputs.mouse_button = 0;
            }
            if (event.button.button == SDL_BUTTON_RIGHT) {
                inputs.mouse_special_attack = TRUE;
                inputs.mouse_button = 1;
            }
            inputs.mouse_x = event.button.x;
            inputs.mouse_y = event.button.y;
        }
    }
    return inputs;
}