#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <SDL/SDL.h>

int bRand(int min, int max);

void freeSurface(SDL_Surface **surface);

int twoblock_col(
    const int bb1x, const int bb1y, const int bb1w, const int bb1h,
    const int bb2x, const int bb2y, const int bb2w, const int bb2h
);

#ifdef __cplusplus
}
#endif

#endif /* UTILS_H */