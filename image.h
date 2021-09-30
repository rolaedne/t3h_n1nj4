#ifndef IMAGE_H
#define IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

SDL_Surface* loadImageAsSurface(const char* filename);
SDL_Surface* createSurface(const unsigned int width, const unsigned int height);
SDL_Surface* mirrorSurface(SDL_Surface *surface);
void freeSurface(SDL_Surface **surface);

#ifdef __cplusplus
}
#endif

#endif /* PARTICLES_H */
