#include <SDL2/SDL_image.h>
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

SDL_Surface *create_rgba_surface(const unsigned int width, const unsigned int height) {
    SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, rmask, gmask, bmask, amask);
    if (surface == NULL) {
        fprintf(stderr, "SDL_CreateRGBSurface failed: %s\n", SDL_GetError());
        exit(3);
    }
    return surface;
}

SDL_Surface* copy_to_rgba_surface(SDL_Surface *source) {
    if (source == NULL) { return NULL; }
    SDL_Surface *original_copy = SDL_ConvertSurface(source, source->format, 0); // don't pollute the original
    SDL_SetSurfaceBlendMode(original_copy, SDL_BLENDMODE_NONE);
    SDL_Surface *rgba_copy = create_rgba_surface(source->w, source->h);
    SDL_BlitSurface(original_copy, NULL, rgba_copy, NULL);
    SDL_FreeSurface(original_copy);
    return rgba_copy;
}

SDL_Surface* load_image_as_rgba(const char* filename) {
    if (filename == NULL) { fprintf(stderr, "load_image_as_rgba() called with NULL filename\n"); exit(1); }

    SDL_Surface *temp = IMG_Load(filename);
    if (temp == NULL) { fprintf(stderr, "Error loading image \"%s\": %s\n", filename, IMG_GetError()); exit(2); }

    SDL_Surface *image = copy_to_rgba_surface(temp);
    SDL_FreeSurface(temp);

    return image;
}

void free_surface(SDL_Surface **surface) {
    if (*(surface) != NULL) {
        SDL_FreeSurface(*(surface));
    }
    *(surface) = NULL;
}

SDL_Surface* mirror_surface(SDL_Surface *source) {
    if (source == NULL) { return NULL; }

    SDL_Surface* copy = copy_to_rgba_surface(source);

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

SDL_Surface* rotate_90(SDL_Surface *source) {
    if (source == NULL) { return NULL; }

    SDL_Surface* original_copy = copy_to_rgba_surface(source);
    SDL_Surface *dest = create_rgba_surface(original_copy->h, original_copy->w);

    if (SDL_MUSTLOCK(original_copy)) { SDL_LockSurface(original_copy); }
    if (SDL_MUSTLOCK(dest)) { SDL_LockSurface(dest); }

    const Uint32 *src_pixels = (Uint32 *) original_copy->pixels;
    Uint32 *dst_pixels = (Uint32 *) dest->pixels;
    const int sw = original_copy->w;
    const int sh = original_copy->h;
    const int dw = dest->w;
    const int sy_max = sh - 1;
    for (int sy = 0; sy < sh; ++sy) {
        const int dx = sy_max - sy;
        for (int sx = 0; sx < sw; ++sx) {
            const int dy = sx;
            dst_pixels[ (dy * dw) + dx ] = src_pixels[ (sy * sw) + sx ];
        }
    }

    if (SDL_MUSTLOCK(original_copy)) { SDL_UnlockSurface(original_copy); }
    if (SDL_MUSTLOCK(dest)) { SDL_UnlockSurface(dest); }

    SDL_FreeSurface(original_copy);

    return dest;
}

SDL_Surface* rotate_180(SDL_Surface *source) {
    if (source == NULL) { return NULL; }

    SDL_Surface* original_copy = copy_to_rgba_surface(source);
    SDL_Surface *dest = create_rgba_surface(original_copy->w, original_copy->h);

    if (SDL_MUSTLOCK(original_copy)) { SDL_LockSurface(original_copy); }
    if (SDL_MUSTLOCK(dest)) { SDL_LockSurface(dest); }

    const Uint32 *src_pixels = (Uint32 *) original_copy->pixels;
    Uint32 *dst_pixels = (Uint32 *) dest->pixels;
    const int sw = original_copy->w;
    const int sh = original_copy->h;
    const int dw = dest->w;
    const int sx_max = sw - 1;
    const int sy_max = sh - 1;
    for (int sy = 0; sy < sh; ++sy) {
        const int dy = sy_max - sy;
        for (int sx = 0; sx < sw; ++sx) {
            const int dx = sx_max - sx;
            dst_pixels[ (dy * dw) + dx ] = src_pixels[ (sy * sw) + sx ];
        }
    }

    if (SDL_MUSTLOCK(original_copy)) { SDL_UnlockSurface(original_copy); }
    if (SDL_MUSTLOCK(dest)) { SDL_UnlockSurface(dest); }

    SDL_FreeSurface(original_copy);

    return dest;
}

SDL_Surface* rotate_270(SDL_Surface *source) {
    if (source == NULL) { return NULL; }

    SDL_Surface* original_copy = copy_to_rgba_surface(source);
    SDL_Surface *dest = create_rgba_surface(original_copy->h, original_copy->w);

    if (SDL_MUSTLOCK(original_copy)) { SDL_LockSurface(original_copy); }
    if (SDL_MUSTLOCK(dest)) { SDL_LockSurface(dest); }

    const Uint32 *src_pixels = (Uint32 *) original_copy->pixels;
    Uint32 *dst_pixels = (Uint32 *) dest->pixels;
    const int sw = original_copy->w;
    const int sh = original_copy->h;
    const int dw = dest->w;
    const int sx_max = sw - 1;
    for (int sy = 0; sy < sh; ++sy) {
        const int dx = sy;
        for (int sx = 0; sx < sw; ++sx) {
            const int dy = sx_max - sx;
            dst_pixels[ (dy * dw) + dx ] = src_pixels[ (sy * sw) + sx ];
        }
    }

    if (SDL_MUSTLOCK(original_copy)) { SDL_UnlockSurface(original_copy); }
    if (SDL_MUSTLOCK(dest)) { SDL_UnlockSurface(dest); }

    SDL_FreeSurface(original_copy);

    return dest;
}
