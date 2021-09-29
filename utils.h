#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <SDL/SDL.h>

typedef enum {
    FALSE = 0, TRUE = 1
} BOOLEAN;

int bRand(int min, int max);

void freeSurface(SDL_Surface **surface);

int twoblock_col(
    const int bb1x, const int bb1y, const int bb1w, const int bb1h,
    const int bb2x, const int bb2y, const int bb2w, const int bb2h
);

int delayMsNoSkip(const unsigned int msToDelay);
int delayMs(const unsigned int);

typedef struct {
    int x;
    int y;
    int w;
    int h;
} bbox;

int bbox_col(bbox box1, bbox box2);

SDL_Event event;

#ifdef __cplusplus
}
#endif

#endif /* UTILS_H */