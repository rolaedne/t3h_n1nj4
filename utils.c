#include <stdlib.h>
#include "utils.h"

// Bounded random
int bRand(const int min, const int max) {
    const int random = rand() % max;
    if (random < min) {
        return random + min;
    }
    return random;
}

int twoblock_col(
    const int bb1x, const int bb1y, const int bb1w, const int bb1h,
    const int bb2x, const int bb2y, const int bb2w, const int bb2h
) {
    /*check upper left corner of bb2*/
    if (bb1x <= bb2x && bb2x <= bb1x + bb1w)
        if (bb1y <= bb2y && bb2y <= bb1y + bb1h)
            return 1;

    /*check upper right corner of bb2*/
    if (bb1x <= bb2x + bb2w && bb2x + bb2w <= bb1x + bb1w)
        if (bb1y <= bb2y && bb2y <= bb1y + bb1h)
            return 1;

    /*check bottom left corner of bb2*/
    if (bb1x <= bb2x && bb2x <= bb1x + bb1w)
        if (bb1y <= bb2y + bb2h && bb2y + bb2h <= bb1y + bb1h)
            return 1;

    /*check bottom right corner of bb2*/
    if (bb1x <= bb2x + bb2w && bb2x + bb2w <= bb1x + bb1w)
        if (bb1y <= bb2y + bb2h && bb2y + bb2h <= bb1y + bb1h)
            return 1;
    return 0;
}

int bbox_col(bbox box1, bbox box2) {
    return twoblock_col(
        box1.x, box1.y, box1.w, box1.h,
        box2.x, box2.y, box2.w, box2.h
    );
}

BOOLEAN skippable = TRUE;
int delayMsNoSkip(const unsigned int msToDelay) {
    skippable = FALSE;
    const int result = delayMs(msToDelay);
    skippable = TRUE;
    return result;
}

int delayMs(const unsigned int msToDelay) {
    int msecs_waited = 0;
    while(msecs_waited < msToDelay) {
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