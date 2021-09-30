/*******************************************
 *Roderick Newill
 *nmy.c
 *these function handle movement , attacks and
 *animation of the enemies.
 ********************************************/

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "world.h"
#include "nmy.h"
#include "plyr.h"
#include "particles.h"
#include "utils.h"
#include "screens.h"

// Takes care of case if enemy hands touch ninja. you will make with the dead.
void check_player_collision(enemy *e) {
    if (isDead || !e->is_alive) { return; } // don't kill the player if they're already dead, and dead enemies can't hurt the player
    bbox player_box = { dest.x - 5, dest.y + 5, 50, 70 };
    bbox enemy_box = { e->dest.x - 10, e->dest.y, 50, 50 };
    if (bbox_collision(player_box, enemy_box)) { dead(); }
}

void update_animation_frame(enemy *e) {
    if (e->anim_delay-- <= 0) {
        e->anim_delay = NMYDLY;
        e->anim_frame++;
        if (e->anim_frame >= NMY_FRAMES || e->anim_frames[e->anim_frame] == NULL) { e->anim_frame = 0; }
    }
}

void draw_enemy(enemy *e) {
    const int right_edge = e->dest.x + (BRICK_WIDTH * 2);
    if ((e->dest.x - wrldps.x) < SCREENWIDTH && right_edge > 0) {/*if enemy is alive draw him on screen*/
        if (e->is_alive) {
            e->is_visible = 1;
            e->is_flipped = (e->dest.x < dest.x); // living enemies should face the player
            SDL_Surface *enemy_surface = e->is_flipped ? e->anim_frames_flipped[e->anim_frame] : e->anim_frames[e->anim_frame];
            SDL_BlitSurface(enemy_surface, NULL, screen, &e->dest);
        } else {
            SDL_Surface *death_surface = e->is_flipped ? e->death_frames_flipped[e->death_type] : e->death_frames[e->death_type];
            if (death_surface != NULL) {
                e->is_visible = 1;
                if (e->death_bleed_counter > 0 || rand() % 75 == 0) {
                    spawn_blood_particles(e->dest);
                    e->death_bleed_counter--;
                }
                const int old_x = e->dest.x; // drawing off the edge of a surface (screen) will clip the dest rect
                const int old_y = e->dest.y; // TODO: x, y shouldn't be in an SDL_Rect directly used to draw
                SDL_BlitSurface(death_surface, NULL, screen, &e->dest);
                e->dest.x = old_x;
                e->dest.y = old_y;
            }
        }
    }
}

void enemy_physics(enemy *e) {
    /*****************************************
     *enemy physisc loop
     *collision detection for the enemies
     *return vals used to operat the enemy ai funct
     ******************************************/
    int tmp;

    e->dest.y += GRAVITY + e->gravity_compound;
    e->gravity_compound += GRAVITY;

    e->dir[5] = 1;

    if (e->dest.y + (BRICK_HEIGHT / 2) > SCREENHEIGHT) {
        e->is_alive = 0;
        spawn_blood_particles(e->dest);
    }

    if (world[(e->dest.y + BRICK_HEIGHT - GRAVITY) / BRICK_HEIGHT]
            [(e->dest.x + 10 + wrldps.x) / BRICK_WIDTH] > 0) {/*checks up and down collision*/
        if (e->is_alive != 0 &&
                world[(e->dest.y + BRICK_HEIGHT - GRAVITY) / BRICK_HEIGHT]
                [(e->dest.x + 10 + wrldps.x) / BRICK_WIDTH] == 7) {/*lava check*/
            //spawn_blood_particles(e->nmydest);
            //e->is_alive=0;
            e->dir[0] = 1;
        } else {
            tmp = (((e->dest.y + (BRICK_HEIGHT)) - GRAVITY) / BRICK_HEIGHT) - 1;
            tmp *= BRICK_HEIGHT;
            e->gravity_compound = 0;
            e->dest.y = tmp;
            e->dir[1] = 1;
            e->jump_is_active = 0;
        }
    }
    if (world[(e->dest.y + BRICK_HEIGHT - GRAVITY) / BRICK_HEIGHT]
            [(e->dest.x + BRICK_WIDTH - 5 + wrldps.x) / BRICK_WIDTH] > 0) {/*checks bottom right*/
        if (world[(e->dest.y + BRICK_HEIGHT - GRAVITY) / BRICK_HEIGHT]
                [(e->dest.x + BRICK_WIDTH - 10 + wrldps.x) / BRICK_WIDTH] == 7
                && e->is_alive != 0) {/*lava block collision equals death*/
            e->dir[0] = 1;
        } else {
            tmp = (((e->dest.y + (BRICK_HEIGHT)) - GRAVITY) / BRICK_HEIGHT) - 1;
            tmp *= BRICK_HEIGHT;
            e->gravity_compound = 0;
            e->dest.y = tmp;
            e->dir[3] = 1;
            e->jump_is_active = 0;
        }
    }
    if (world[(e->dest.y) / BRICK_HEIGHT]
            [(e->dest.x + BRICK_WIDTH - 10 + wrldps.x) / BRICK_WIDTH] > 0) {/*check top*/
        tmp = (((e->dest.y + (BRICK_HEIGHT)) - GRAVITY) / BRICK_HEIGHT) - 1;
        tmp *= BRICK_HEIGHT;
        e->gravity_compound = 0;
        e->dest.y = tmp + BRICK_HEIGHT;
        e->dir[8] = 1;
    }
    if (world[(e->dest.y) / BRICK_HEIGHT]
            [(e->dest.x + wrldps.x) / BRICK_WIDTH] > 0) {/*check top*/
        tmp = (((e->dest.y + (BRICK_HEIGHT)) - GRAVITY) / BRICK_HEIGHT) - 1;
        tmp *= BRICK_HEIGHT;
        e->gravity_compound = 0;
        e->dest.y = tmp + BRICK_HEIGHT;
        e->dir[8] = 1;
    }

    if (world[e->dest.y / BRICK_HEIGHT]
            [(e->dest.x + BRICK_WIDTH - 10 + wrldps.x) / BRICK_WIDTH] > 0) {/*checks side to side block collision*/
        e->dest.x -= MOVERL;
        e->dir[9] = 1;
    }
    if (world[e->dest.y / BRICK_HEIGHT][(e->dest.x + wrldps.x) / BRICK_WIDTH] > 0) {
        e->dest.x += MOVERL;
        e->dir[7] = 1;
    }
    if (world[(e->dest.y + 70) / BRICK_HEIGHT]
            [(e->dest.x + BRICK_WIDTH - 1 + wrldps.x) / BRICK_WIDTH] > 0) {
        e->dest.x -= MOVERL;
        e->dir[6] = 1;
    }
    if (world[(e->dest.y + 70) / BRICK_HEIGHT][(e->dest.x + wrldps.x) / BRICK_WIDTH] > 0) {
        e->dest.x += MOVERL;
        e->dir[4] = 1;
    }
    if (world[(e->dest.y + BRICK_HEIGHT - 20) / BRICK_HEIGHT]
            [(e->dest.x + (BRICK_WIDTH / 2) + wrldps.x) / BRICK_WIDTH] == 7
            && e->is_alive != 0) {/*fallen into lava pit*/
        e->is_alive = 0;
        e->death_type = BYLAVA;

        spawn_blood_particles(e->dest);
    }
}/*end nmy physcis*/

void enemy_ai() {
    /***************************************************
     *This is the brain of the enemy. This makes them "think good".
     *Runs through all of their operations to make them
     *function when (an evil?) ninja comes into their world
     ****************************************************/
    for (int i = 0; i < enemymax; i++) {
        enemy *e = &enemies[i];
        enemy_physics(e);
        draw_enemy(e);
        check_player_collision(e);
        update_animation_frame(e);

        if (e->is_visible == 1 && e->is_alive == 1) {/*is the enemy on the screen*/
            if ((e->dir[4] || e->dir[7]) && e->jump_is_active == 0) {/*block on the left*/
                e->gravity_compound = e->jump_strength;
                e->jump_is_active = 1;
            }
            if ((e->dir[9] || e->dir[6])
                    && e->jump_is_active == 0) {/*block on the right*/
                e->gravity_compound = e->jump_strength;
                e->jump_is_active = 1;
            }
            if (e->dir[3] == 1 || e->dir[1] == 1 || e->dir[5] == 1) {/*on the ground or in the air go toward ninja*/
                if (dest.x > e->dest.x) {
                    e->dest.x += e->speed;
                    /*add check for other enemies*/
                } else {
                    e->dest.x -= e->speed;
                }
                if ((e->dir[3] == 1 || e->dir[0] == 1) && e->dir[1] == 0
                        && e->jump_is_active == 0) {/*a pit is ahead* left*/
                    e->gravity_compound = e->jump_strength;
                    e->jump_is_active = 1;
                } else if ((e->dir[3] == 0 || e->dir[0] == 1) && e->dir[1] == 0
                        && e->jump_is_active == 0) {/*a pit is ahead* left*/
                    e->gravity_compound = e->jump_strength;
                    e->jump_is_active = 1;
                }
            }
        } else if (e->is_visible == 1 && e->is_alive == 0 && e->jump_is_active == 1) {
            /* this gives dead enemies a knockback effect */
            if (ninja_src.x < 180) {
                e->dest.x -= 5; /*temp val*/
            } else if (ninja_src.x >= 180) {
                e->dest.x += 5; /*temp val*/
            }
            int x, y;
            x = e->dest.x + e->dest.w / 2;
            y = e->dest.y + e->dest.h / 2;
            x += wrldps.x;
            spawn_particle(blood1, x, y, 0, 0, 1.0, 7);
        }
        for (int t = 0; t < 10; t++) {/*rests their dir arrray*/
            e->dir[t] = 0;
        }
    }/*end for loop*/
}


