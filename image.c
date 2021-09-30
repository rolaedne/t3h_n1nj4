#include "image.h"

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    const Uint32 rmask = 0xff000000;
    const Uint32 gmask = 0x00ff0000;
    const Uint32 bmask = 0x0000ff00;
    const Uint32 amask = 0x000000ff;
#else
    const Uint32 rmask = 0x000000ff;
    const Uint32 gmask = 0x0000ff00;
    const Uint32 bmask = 0x00ff0000;
    const Uint32 amask = 0xff000000;
#endif

SDL_Surface* copySurface(SDL_Surface *surface) {
    if (surface == NULL) { return NULL; }
    SDL_Surface *copy = SDL_ConvertSurface(surface, surface->format, 0); // don't pollute the original
    SDL_SetAlpha(copy, 0, SDL_ALPHA_OPAQUE); // copying with transparent pixels doesn't work how you'd expect!
    SDL_Surface *new_copy = createSurface(surface->w, surface->h);
    SDL_BlitSurface(copy, NULL, new_copy, NULL);
    SDL_FreeSurface(copy);
    return new_copy;
}

SDL_Surface *loadImageRGBA(const char *filename) {
    if (filename == NULL) { fprintf(stderr, "loadImageRGBA() called with NULL filename\n"); exit(1); }

    SDL_Surface *temp = IMG_Load(filename);
    if (temp == NULL) { fprintf(stderr, "Error loading image \"%s\": %s\n", filename, IMG_GetError()); exit(2); }

    SDL_Surface *image = copySurface(temp);
    SDL_FreeSurface(temp);

    return image;
}

SDL_Surface* loadImageAsSurface(const char* filename) {
    return loadImageRGBA(filename);
}

SDL_Surface *createSurface(const unsigned int width, const unsigned int height) {
    SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, rmask, gmask, bmask, amask);
    if (surface == NULL) {
        fprintf(stderr, "SDL_CreateRGBSurface failed: %s\n", SDL_GetError());
        exit(3);
    }
    return surface;
}

void freeSurface(SDL_Surface **surface) {
    if (*(surface) != NULL) {
        SDL_FreeSurface(*(surface));
    }
    *(surface) = NULL;
}

SDL_Surface* mirrorSurface(SDL_Surface *surface) {
    if (surface == NULL) { return NULL; }

    SDL_Surface* copy = copySurface(surface);

    if (SDL_MUSTLOCK(copy)) { SDL_LockSurface(copy); }

    Uint32 *pixels = (Uint32 *) copy->pixels;
    for (int y = 0; y < copy->h; ++y) { // for each row
        const int row_offset = y * copy->w;
        for (int x = 0, mx = copy->w - 1; x < mx; ++x, --mx) {
            const Uint32 pixel_x = pixels[ row_offset + x ];
            const Uint32 pixel_mx = pixels[ row_offset + mx ];
            pixels[ row_offset + x ] = pixel_mx;
            pixels[ row_offset + mx ] = pixel_x;
        }
    }

    if (SDL_MUSTLOCK(copy)) { SDL_UnlockSurface(copy); }

    return copy;
}
