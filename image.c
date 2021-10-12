#include <SDL2/SDL_image.h>
#include "image.h"
#include "utils.h"

SDL_Surface* rotate_90(SDL_Surface *source);
SDL_Surface* rotate_180(SDL_Surface *source);
SDL_Surface* rotate_270(SDL_Surface *source);

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

SDL_Surface* as_rgba_surface(SDL_Surface *source, const Uint32 format) {
    if (source == NULL) { return NULL; }
    if (source->format->format == format) { return source; }
    return copy_to_rgba_surface(source);
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

typedef struct {
    int x;
    int y;
} XY;

XY sheer(const int x, const int y, const double sin_a, const double tan_half_a) {
    int new_x = x - (y * tan_half_a); // sheer 1
    int new_y = (new_x * sin_a) + y; // sheer 2
    new_x = new_x - (new_y * tan_half_a); // sheer 3
    return (XY){ .x = new_x, .y = new_y };
}

SDL_Surface* rotate(SDL_Surface *source, const int angle_degrees) {
    if (source == NULL) { return NULL; }

    int modified_angle = abs(angle_degrees) % 360;
    if (angle_degrees < 0) { modified_angle = 360 - modified_angle; } // convert negative degrees into their matching positive versions

    const int octant = modified_angle / 45;
    while (modified_angle > 45) { modified_angle -= 45; }
    if (octant % 2 == 1 && modified_angle != 45) { modified_angle = (45 - modified_angle); }
    if (octant % 2 == 1) { modified_angle = -modified_angle; }

    //printf("Debug: angle: %d - octant: %d, modified angle: %d\n", angle_degrees, octant, modified_angle);
    // Perfect rotation available? // this is down here so it can be after the debug statement above
    if (angle_degrees % 90 == 0) {
        switch (angle_degrees / 90) {
            case 0: return copy_to_rgba_surface(source);
            case 1: return rotate_90(source);
            case 2: return rotate_180(source);
            case 3: return rotate_270(source);
        }
    }

    const double angle_radians = modified_angle * (M_PI / 180.0);
    const double cos_a = cos(angle_radians);
    const double sin_a = sin(angle_radians);
    const double tan_half_a = tan(angle_radians / 2.0);

    SDL_Surface *src = NULL;

    switch(octant) {
        case 7: // 0    - (modified angle % 45)
        case 0: // 0    + (modified angle % 45)
            src = source; //as_rgba_surface(source, dest->format->format); // need dest before getting the format
            break;
        case 1: // 90   - (modified angle % 45)
        case 2: // 90   + (modified angle % 45)
            src = rotate_90(source);
            break;
        case 3: // 180  - (modified angle % 45)
        case 4: // 180  + (modified angle % 45)
            src = rotate_180(source);
            break;
        case 5: // 270  - (modified angle % 45)
        case 6: // 270  + (modified angle % 45)
            src = rotate_270(source);
    }

    const int new_h = fabs(src->h * cos_a) + fabs(src->w * sin_a) + 1;
    const int new_w = fabs(src->h * sin_a) + fabs(src->w * cos_a) + 1;

    SDL_Surface *dest = create_rgba_surface(new_w, new_h);
    if (src == source) { src = as_rgba_surface(source, dest->format->format); } // need dest's format

    if (SDL_MUSTLOCK(src)) { SDL_LockSurface(src); }
    if (SDL_MUSTLOCK(dest)) { SDL_LockSurface(dest); }

    const Uint32 *src_pixels = (Uint32 *) src->pixels;
    Uint32 *dst_pixels = (Uint32 *) dest->pixels;

    const int sx_max = src->w - 1;
    const int sy_max = src->h - 1;
    const int src_center_x = src->w / 2;
    const int src_center_y = src->h / 2;
    const int dst_center_x = dest->w / 2;
    const int dst_center_y = dest->h / 2;

    for (int sy = 0; sy < src->h; ++sy) {
        const int s_row_offset = sy * src->w;
        const int y = sy_max - sy - src_center_y;
        for (int sx = 0; sx < src->w; ++sx) {
            const int x = sx_max - sx - src_center_x;
            const XY sheered_xy = sheer(x, y, sin_a, tan_half_a);
            const int dx = dst_center_x - sheered_xy.x;
            const int dy = dst_center_y - sheered_xy.y;
            dst_pixels[ (dy * dest->w) + dx ] = src_pixels[ s_row_offset + sx ];
        }
    }

    if (SDL_MUSTLOCK(src)) { SDL_UnlockSurface(src); }
    if (SDL_MUSTLOCK(dest)) { SDL_UnlockSurface(dest); }

    if (src != source) { SDL_FreeSurface(src); }

    return dest;
}


SDL_Surface* rotate_90(SDL_Surface *source) {
    if (source == NULL) { return NULL; }

    SDL_Surface *dest = create_rgba_surface(source->h, source->w);
    SDL_Surface *src = as_rgba_surface(source, dest->format->format);

    if (SDL_MUSTLOCK(src)) { SDL_LockSurface(src); }
    if (SDL_MUSTLOCK(dest)) { SDL_LockSurface(dest); }

    const Uint32 *src_pixels = (Uint32 *) src->pixels;
    Uint32 *dst_pixels = (Uint32 *) dest->pixels;
    const int sw = src->w;
    const int sh = src->h;
    const int dw = dest->w;
    const int sy_max = sh - 1;
    for (int sy = 0; sy < sh; ++sy) {
        const int dx = sy_max - sy;
        for (int sx = 0; sx < sw; ++sx) {
            const int dy = sx;
            dst_pixels[ (dy * dw) + dx ] = src_pixels[ (sy * sw) + sx ];
        }
    }

    if (SDL_MUSTLOCK(src)) { SDL_UnlockSurface(src); }
    if (SDL_MUSTLOCK(dest)) { SDL_UnlockSurface(dest); }

    if (src != source) { SDL_FreeSurface(src); }

    return dest;
}

SDL_Surface* rotate_180(SDL_Surface *source) {
    if (source == NULL) { return NULL; }

    SDL_Surface *dest = create_rgba_surface(source->w, source->h);
    SDL_Surface *src = as_rgba_surface(source, dest->format->format);

    if (SDL_MUSTLOCK(src)) { SDL_LockSurface(src); }
    if (SDL_MUSTLOCK(dest)) { SDL_LockSurface(dest); }

    const Uint32 *src_pixels = (Uint32 *) src->pixels;
    Uint32 *dst_pixels = (Uint32 *) dest->pixels;
    const int sw = src->w;
    const int sh = src->h;
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

    if (SDL_MUSTLOCK(src)) { SDL_UnlockSurface(src); }
    if (SDL_MUSTLOCK(dest)) { SDL_UnlockSurface(dest); }

    if (src != source) { SDL_FreeSurface(src); }

    return dest;
}

SDL_Surface* rotate_270(SDL_Surface *source) {
    if (source == NULL) { return NULL; }

    SDL_Surface *dest = create_rgba_surface(source->h, source->w);
    SDL_Surface *src = as_rgba_surface(source, dest->format->format);

    if (SDL_MUSTLOCK(src)) { SDL_LockSurface(src); }
    if (SDL_MUSTLOCK(dest)) { SDL_LockSurface(dest); }

    const Uint32 *src_pixels = (Uint32 *) src->pixels;
    Uint32 *dst_pixels = (Uint32 *) dest->pixels;
    const int sw = src->w;
    const int sh = src->h;
    const int dw = dest->w;
    const int sx_max = sw - 1;
    for (int sy = 0; sy < sh; ++sy) {
        const int dx = sy;
        for (int sx = 0; sx < sw; ++sx) {
            const int dy = sx_max - sx;
            dst_pixels[ (dy * dw) + dx ] = src_pixels[ (sy * sw) + sx ];
        }
    }

    if (SDL_MUSTLOCK(src)) { SDL_UnlockSurface(src); }
    if (SDL_MUSTLOCK(dest)) { SDL_UnlockSurface(dest); }

    if (src != source) { SDL_FreeSurface(src); }

    return dest;
}
