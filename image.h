#ifndef IMAGE_H
#define IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <SDL2/SDL.h>

SDL_Surface* load_image_as_rgba(const char* filename);
SDL_Surface* create_rgba_surface(const unsigned int width, const unsigned int height);
SDL_Surface* mirror_surface(SDL_Surface *surface);
void free_surface(SDL_Surface **surface);

#ifdef __cplusplus
}
#endif

#endif /* PARTICLES_H */
