#include "particles.h"
#include "world.h"
#include <stddef.h>
#include <SDL/SDL_video.h>

//TODO: make this a collection
#define MAX_PARTICLES 250
Particle particles[MAX_PARTICLES];

int gravity_accumulator = 3;
int max_y_vel = 25;

void applyParticlePhysics(Particle *p);

void addParticle(SDL_Surface *image, int x, int y, int x_vel, int y_vel, float weight, int size) {
    Particle *p = NULL;
    int i;
    for (i=0; i < MAX_PARTICLES; ++i) {
        if (particles[i].image == NULL) {
            p = &particles[i];
            break;
        }
    }
    if (p == NULL) {
        printf("Could not find free particle slot\n");
        return;
    }
    if (weight > 1.0) { weight = 1.0; };
    if (weight < 0.0) { weight = 0.0; };
    p->image = image;
    p->x = x;
    p->y = y;
    p->x_vel = x_vel;
    p->y_vel = y_vel;
    p->weight = weight;
    p->size = size;
}

void drawParticle(SDL_Surface *screen, Particle *p) {
    SDL_Rect src; src.x = p->size; src.y = p->size; src.w = p->size; src.h = p->size;
    SDL_Rect dest; dest.x = p->x - wrldps.x; dest.y = p->y;
    SDL_BlitSurface(p->image, &src, screen, &dest);
}

void persistToBackground(Particle *p) {
    p->x += wrldps.x;
    drawParticle(background, p);
    p->image = NULL;
}

void drawParticles(SDL_Surface *screen) {
    Particle *p = NULL;
    int i;
    for (i=0; i < MAX_PARTICLES; ++i) {
        if (particles[i].image != NULL) {
            p = &particles[i];
            applyParticlePhysics(p);
            if (isCollision(p->x, p->y)) {
                persistToBackground(p);
            } else {
                drawParticle(screen, p);
            }
        }
    }
}

void applyParticlePhysics(Particle *p) {
    if (p->y_vel > max_y_vel) {
        p->y_vel = max_y_vel;
    }
    p->y += p->y_vel;
    p->y_vel += (gravity_accumulator * p->weight);
    p->x += p->x_vel;
}
