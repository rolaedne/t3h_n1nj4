#include "image.h"

SDL_Surface* loadImageAsSurface(const char* filename) {
    SDL_Surface *img = IMG_Load(filename);
    if (NULL == img) {
        fprintf(stderr, "Error loading image: %s - %s\n", filename, IMG_GetError());
        exit(EXIT_FAILURE);
    }
    return img;
}

SDL_Surface *createSurface(const unsigned int width, const unsigned int height) {
    Uint32 rmask, gmask, bmask, amask;
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0xff000000;
        gmask = 0x00ff0000;
        bmask = 0x0000ff00;
        amask = 0x000000ff;
    #else
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = 0xff000000;
    #endif
    return SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, rmask, gmask, bmask, amask);
}
