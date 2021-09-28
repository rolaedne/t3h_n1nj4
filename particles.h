#ifndef PARTICLES_H
#define PARTICLES_H

#ifdef __cplusplus
extern "C" {
#endif
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

typedef struct Particle {
    int x;
    int y;
    int x_vel;
    int y_vel;
    float weight;
    int size;
    SDL_Surface *image;
} Particle;

void addParticle(SDL_Surface *image, int x, int y, int x_vel, int y_vel, float weight, int size);
void drawParticles(SDL_Surface *screen);

#ifdef __cplusplus
}
#endif

#endif /* PARTICLES_H */
