#ifndef PARTICLES_H
#define PARTICLES_H

#ifdef __cplusplus
extern "C" {
#endif
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

typedef struct {
    int x;
    int y;
    int x_vel;
    int y_vel;
    float weight;
    int size;
    SDL_Surface *image;
} Particle;

void spawn_particle(SDL_Surface *image, const int x, const int y, const int x_vel, const int y_vel, const float weight, const int size);
void draw_particles(SDL_Surface *screen);
void clear_particles();

#ifdef __cplusplus
}
#endif

#endif /* PARTICLES_H */
