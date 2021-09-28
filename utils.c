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


void freeSurface(SDL_Surface **surface) {
    if (*(surface) != NULL) {
        SDL_FreeSurface(*(surface));
    }
    *(surface) = NULL;
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