#include <stdlib.h>
#include "utils.h"

// Bounded random
int bounded_rand(const int min, const int max) {
    const int random = rand() % max;
    if (random < min) {
        return random + min;
    }
    return random;
}

int bbox_collision(const bbox box1, const bbox box2) {
    return (box1.x < box2.x + box2.w &&
            box1.x + box1.w > box2.x &&
            box1.y < box2.y + box2.h &&
            box1.y + box1.h > box2.y
    );
}

Boolean skippable = TRUE;
int delay_ms_unskippable(const unsigned int ms_to_delay) {
    skippable = FALSE;
    const int result = delay_ms_skippable(ms_to_delay);
    skippable = TRUE;
    return result;
}

int delay_ms_skippable(const unsigned int ms_to_delay) {
    unsigned int msecs_waited = 0;
    while(msecs_waited < ms_to_delay) {
        msecs_waited += 10;
        SDL_Delay(10);
        if (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    exit(EXIT_SUCCESS);
                    break; /* unreachable, but here for clarity */
                case SDL_KEYUP:
                    if (!skippable) { continue; }
                    if (event.key.keysym.sym == SDLK_SPACE) {
                        return 1;
                    }
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        exit(EXIT_SUCCESS);
                    }
                    continue;
                default:
                    continue;
            }
        }
    }
    return 0;
}