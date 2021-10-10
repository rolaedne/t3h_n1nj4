#include <SDL2/SDL.h>
#include "special.h"
#include "plyr.h"
#include "utils.h"
#include "image.h"
#include "nmy.h"
#include "world.h"

#define SPECIAL_ATTACK_FRAMES 2
#define SPECIAL_ATTACK_TYPES 2
#define MAX_SPECIAL_ATTACKS 16

typedef struct {
    Boolean is_active;
    Special_Type type;
    int x;
    int y;
    int w;
    int h;
    int x_vel;
    int y_vel;
    int anim_frame;
    int attack_length;
} Special;

Special specials[MAX_SPECIAL_ATTACKS];

SDL_Surface *special_attack_frames[SPECIAL_ATTACK_TYPES][SPECIAL_ATTACK_FRAMES];
SDL_Surface *special_attack_frames_flipped[SPECIAL_ATTACK_TYPES][SPECIAL_ATTACK_FRAMES];
const int special_attack_speeds[SPECIAL_ATTACK_TYPES] = { 25, 15 };

void special_attack_check_for_kills(Special *s) {
    if (!s->is_active) { return; }
    const bbox sattackBox = { s->x + vp.x, s->y + vp.y, s->w, s->h }; // TODO: fix the special x,y to be world coordinates instead of screen coordinates
    for (int i = 0; i < NMY; i++) {/*checks for each enemy*/
        enemy *e = &enemies[i];
        if (s->type == STAR && !e->is_alive) { continue; } // don't recheck the dead for stars
        if (s->type == FIRE && !e->is_alive && e->death_type == BYLAVA) { continue; } // don't re-burn corpses

        if (!bbox_collision((bbox) { e->x, e->y, e->w, e->h }, sattackBox)) { continue; } // no collision, move on

        e->is_alive = 0;
        if (s->type == STAR) {
            e->death_type = BYSTAR;
            e->death_bleed_counter = 10; // how bloody is the death?
        } else if (s->type == FIRE) {
            e->death_type = BYLAVA;
            e->death_bleed_counter = 5; // how bloody is the death?
        }
        player.score += 5; // small refund for star kills
        s->is_active = 0;
        return;
    }
}

void init_special_attack() {
    special_attack_frames[STAR][0] = load_image_as_rgba("weapons/star1.png");
    special_attack_frames[STAR][1] = load_image_as_rgba("weapons/star2.png");
    special_attack_frames[FIRE][0] = load_image_as_rgba("weapons/fireball1.png");
    special_attack_frames[FIRE][1] = load_image_as_rgba("weapons/fireball2.png");
    for (int type = 0; type < SPECIAL_ATTACK_TYPES; ++type) {
        for (int frame = 0; frame < SPECIAL_ATTACK_FRAMES; ++frame) {
            special_attack_frames_flipped[type][frame] = mirror_surface(special_attack_frames[type][frame]);
        }
    }
}

void clear_special_attack() {
    for (int i = 0; i < MAX_SPECIAL_ATTACKS; ++i) {
        specials[i].is_active = FALSE;
    }
}

Special* find_special_slot() {
    for (int i=0; i < MAX_SPECIAL_ATTACKS; ++i) {
        if (!specials[i].is_active) { return &specials[i]; }
    }
    return NULL;
}


void special_attack(const Special_Type type, const int source_x, const int source_y, const int mouse_x, const int mouse_y) {
    //printf("Special attack! type: %d %d, %d\n", type, mouse_x, mouse_y);

    if (type < SPECIAL_ATTACK_TYPES && player.score >= 0) { // allow specials with no score for now, and allow rethrowing
        player.score -= 10; if (player.score < 0) { player.score = 0; }
        Special *s = find_special_slot();
        if (s == NULL) {
            printf("Could not find free special slot\n");
            return;
        }
        s->type = type;
        s->w = special_attack_frames[type][0]->w;
        s->h = special_attack_frames[type][0]->h;

        s->x = source_x - (s->w / 2);
        s->y = source_y + (s->h / 2);
        s->attack_length = 25;
        s->is_active = TRUE;
        const int speed = special_attack_speeds[type];

        if (mouse_x == -1 || mouse_y == -1) {
            s->x_vel = player.is_facing_right ? speed : -speed;
            s->y_vel = 0;
        } else {
            const long x_diff = s->x - mouse_x;
            const long y_diff = s->y - mouse_y;
            if (x_diff == 0) {
                s->x_vel = 0;
                s->y_vel = y_diff > 0 ? speed : -speed;
            } else if (y_diff == 0) {
                s->x_vel = x_diff > 0 ? speed : -speed;
                s->y_vel = 0;
            } else {
                // x_diff and y_diff are both non-zero, we can do division
                const double angle = atan((double) x_diff / (double) y_diff);
                const double new_x_diff = speed * sin(angle);
                const double new_y_diff = speed * cos(angle);
                s->x_vel = abs((int) new_x_diff);
                if (mouse_x < s->x) { s->x_vel = -s->x_vel; }
                s->y_vel = abs((int) new_y_diff);
                if (mouse_y < s->y) { s->y_vel = -s->y_vel; }
            }
        }
        special_attack_check_for_kills(s);
    }
}

void special_attack_tick() {
    for (int i = 0; i < MAX_SPECIAL_ATTACKS; ++i) {
        Special *s = &specials[i];
        if (!s->is_active) { continue; }
        if (--s->attack_length <= 0) {
            s->is_active = FALSE;
            continue;
        }
        s->x += s->x_vel;
        s->y += s->y_vel + 1; // the +1 is for a "gravity" drift downwards
        special_attack_check_for_kills(s);
    }
}

void draw_special_attack(SDL_Surface *screen) {
    for (int i = 0; i < MAX_SPECIAL_ATTACKS; ++i) {
        Special *s = &specials[i];
        if (s->is_active) {
            const Boolean flipped = s->x_vel < 0;
            if (++s->anim_frame >= SPECIAL_ATTACK_FRAMES || special_attack_frames[s->anim_frame] == NULL) { s->anim_frame = 0; }
            SDL_Surface *frame = flipped ? special_attack_frames_flipped[s->type][s->anim_frame] : special_attack_frames[s->type][s->anim_frame];
            SDL_Rect dest = { .x = s->x, .y = s->y };
            SDL_BlitSurface(frame, NULL, screen, &dest);
        }
    }
}
