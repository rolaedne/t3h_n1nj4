#ifndef IMAGE_H
#define IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

SDL_Surface* loadImageAsSurface(const char* filename);

#ifdef __cplusplus
}
#endif

#endif /* PARTICLES_H */
