#include "particles.h"
#include "world.h"
#include <stddef.h>
#include <SDL/SDL_video.h>

//TODO: make this a collection
#define MAX_PARTICLES 1024
Particle particles[MAX_PARTICLES];

const int gravity_accumulator = 3;
const int max_y_vel = 25;

void draw_particle(SDL_Surface *screen, Particle *p) {
    SDL_Rect src; src.x = p->size; src.y = p->size; src.w = p->size; src.h = p->size;
    SDL_Rect dest; dest.x = p->x - wrldps.x; dest.y = p->y;
    SDL_BlitSurface(p->image, &src, screen, &dest);
}

void persist_to_background(Particle *p) {
    p->x += wrldps.x;
    draw_particle(background, p);
    p->image = NULL;
}

void apply_particle_physics(Particle *p) {
    if (p->y_vel > max_y_vel) { p->y_vel = max_y_vel; }
    p->y += p->y_vel;
    p->y_vel += (gravity_accumulator * p->weight);
    p->x += p->x_vel;
}

void spawn_particle(SDL_Surface *image, const int x, const int y, const int x_vel, const int y_vel, const float weight, const int size) {
    Particle *p = NULL;
    for (int i=0; i < MAX_PARTICLES; ++i) {
        if (particles[i].image == NULL) {
            p = &particles[i];
            break;
        }
    }
    if (p == NULL) {
        printf("Could not find free particle slot\n");
        return;
    }
    p->image = image;
    p->x = x;
    p->y = y;
    p->x_vel = x_vel;
    p->y_vel = y_vel;
    p->weight = weight;
    p->size = size;
}

void draw_particles(SDL_Surface *screen) {
    for (int i=0; i < MAX_PARTICLES; ++i) {
        if (particles[i].image != NULL) {
            Particle *p = &particles[i];
            apply_particle_physics(p);
            if (is_collision(p->x, p->y)) {
                persist_to_background(p);
            } else {
                draw_particle(screen, p);
            }
        }
    }
}

void clear_particles() {
    for (int i = 0; i < MAX_PARTICLES; ++i) { particles[i].image = NULL; }
}
