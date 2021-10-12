/*******************************************
 *Roderick Newill
 *nmy.c
 *these function handle movement , attacks and
 *animation of the enemies.
 ********************************************/

#include <SDL2/SDL.h>

#include "world.h"
#include "nmy.h"
#include "plyr.h"
#include "particles.h"
#include "utils.h"
#include "screens.h"

// Takes care of case if enemy hands touch ninja. you will make with the dead.
void check_player_collision(enemy *e) {
    if (player.is_dead || !e->is_alive) { return; } // don't kill the player if they're already dead, and dead enemies can't hurt the player
    const bbox player_box = get_player_hit_box(&player);
    const bbox enemy_attack_box = get_enemy_attack_box(e);
    if (bbox_collision(player_box, enemy_attack_box)) { dead(); }
}

void update_animation_frame(enemy *e) {
    if (e->anim_delay-- <= 0) {
        e->anim_delay = NMYDLY;
        e->anim_frame++;
        if (e->anim_frame >= NMY_FRAMES || e->anim_frames[e->anim_frame] == NULL) { e->anim_frame = 0; }
    }
}

SDL_Surface *get_animation_frame(const enemy *e) {
    if (e->is_alive) {
        return e->is_flipped ? e->anim_frames_flipped[e->anim_frame] : e->anim_frames[e->anim_frame];
    } else {
        return e->is_flipped ? e->death_frames_flipped[e->death_type] : e->death_frames[e->death_type];
    }
}

void draw_enemy(SDL_Surface *screen, enemy *e) {
    //const int right_edge = e->x + (e->w * 2);
    const int right_edge = e->x + (e->w * 2);
    if ((e->x - vp.x) < vp.w && right_edge > vp.x) { // if enemy is alive draw him on screen
        //SDL_Rect dest = { e->x, e->y, e->w, e->h };
        SDL_Rect dest = { e->x - vp.x, e->y - vp.y, e->w, e->h };
        SDL_Surface *surface = get_animation_frame(e);
        if (e->is_alive) {
            e->is_visible = 1;
            SDL_BlitSurface(surface, NULL, screen, &dest);
        } else {
            if (surface != NULL) {
                e->is_visible = 1;
                if (e->death_bleed_counter > 0 || rand() % 75 == 0) {
                    spawn_blood_particles(get_enemy_box(e));
                    e->death_bleed_counter--;
                }
                SDL_BlitSurface(surface, NULL, screen, &dest);
            }
        }
    }
}

int get_tile(const int row, const int col) {
    if (row > WORLD_ROWS || row < 0 || col > WORLD_COLS || col < 0) { return -1; }
    return world[row][col];
}

void populate_dir(enemy *e) {
    //const int center_x = e->x + wrldps.x + (e->w / 2);
    const int center_x = e->x + (e->w / 2);
    const int center_y = e->y + (e->h / 2);
    const int tile_row = center_y / BRICK_HEIGHT;
    const int tile_col = center_x / BRICK_WIDTH;

    /*
    * e->dir[] represents the tiles around the player
    * 7 8 9
    * 4 e 6
    * 1 2 3
    */
    e->dir[7] = get_tile(tile_row - 1, tile_col - 1);
    e->dir[8] = get_tile(tile_row - 1, tile_col);
    e->dir[9] = get_tile(tile_row - 1, tile_col + 1);
    e->dir[4] = get_tile(tile_row, tile_col - 1);
    e->dir[5] = get_tile(tile_row, tile_col); // current position
    e->dir[6] = get_tile(tile_row, tile_col + 1);
    e->dir[1] = get_tile(tile_row + 1, tile_col - 1);
    e->dir[2] = get_tile(tile_row + 1, tile_col);
    e->dir[3] = get_tile(tile_row + 1, tile_col + 1);
}

/*****************************************
 * enemy physisc loop
 * collision detection for the enemies
 * return vals used to operat the enemy ai funct
******************************************/
void enemy_physics(enemy *e) {
    e->y += GRAVITY + e->gravity_compound;
    e->gravity_compound += GRAVITY;

    // death by falling off screen
    if (e->y + (BRICK_HEIGHT / 2) > (vp.max_y + vp.h)) {
        e->is_alive = FALSE;
        spawn_blood_particles(get_enemy_box(e));
    }

    // check bottom-left (dir[1])
    //int test_x = e->x + wrldps.x + 10;
    int test_x = e->x + 10;
    int test_y = e->y + BRICK_HEIGHT - GRAVITY;
    int collision_type = is_collision(test_x, test_y);
    if (collision_type) {
        e->gravity_compound = 0;
        e->is_jumping = FALSE;
        e->y = ((test_y / BRICK_HEIGHT) - 1) * BRICK_HEIGHT;
        test_y = e->y + BRICK_HEIGHT - GRAVITY;
    }

    // check bottom-right (dir[3])
    //test_x = e->x + wrldps.x + BRICK_WIDTH - 10;
    test_x = e->x + BRICK_WIDTH - 10;
    collision_type = is_collision(test_x, test_y);
    if (collision_type) {
        e->gravity_compound = 0;
        e->is_jumping = FALSE;
        e->y = ((test_y / BRICK_HEIGHT) - 1) * BRICK_HEIGHT;
        test_y = e->y + BRICK_HEIGHT - GRAVITY;
    }

    // check top/top-right (dir[8]/dir[9])
    collision_type = is_collision(test_x, e->y);
    if (collision_type) {
        e->gravity_compound = 0;
        e->y = (test_y / BRICK_HEIGHT) * BRICK_HEIGHT;
        test_y = e->y + BRICK_HEIGHT - GRAVITY;
    }

    // check top/top-left (dir[8]/dir[7])
    //collision_type = is_collision(e->x + wrldps.x + 10, e->y);
    collision_type = is_collision(e->x + 10, e->y);
    if (collision_type) {
        e->gravity_compound = 0;
        e->y = (test_y / BRICK_HEIGHT) * BRICK_HEIGHT;
        test_y = e->y + BRICK_HEIGHT - GRAVITY;
    }

    // check right (dir[6])
    //test_x = e->x + wrldps.x + BRICK_WIDTH - 1;
    test_x = e->x + BRICK_WIDTH - 1;
    test_y = e->y + 70;
    collision_type = is_collision(test_x, test_y);
    if (collision_type) {
        e->x -= MOVERL;
    }

    // check left (dir[4])
    //collision_type = is_collision(e->x + wrldps.x, test_y);
    collision_type = is_collision(e->x, test_y);
    if (collision_type) {
        e->x += MOVERL;
    }

    // check below (dir[2])
    //collision_type = is_collision(e->x + wrldps.x + (BRICK_WIDTH / 2), e->y + BRICK_HEIGHT + 10);
    collision_type = is_collision(e->x + (BRICK_WIDTH / 2), e->y + BRICK_HEIGHT + 10);
    if (collision_type == 7) { // fallen into lava pit
        e->is_alive = 0;
        if (e->death_type != BYLAVA) {
            e->death_type = BYLAVA;
            e->death_bleed_counter = 15;
            spawn_blood_particles(get_enemy_box(e));
            // TODO: spawn fire and/or ash particles on lava
        }
    }

    if (e->is_alive) {
        e->is_flipped = (e->x < player.x); // living enemies should face the player
    }
}

/*********************
 * e->dir[] keeps track of the tiles around the enemy.
 * used mainly for ai to handle direction and jumping.
 * position 5 (e) is the enemy's current position. position 0 is ignored.
 * 7 8 9
 * 4 e 6
 * 1 2 3
 * This is the brain of the enemy. This makes them "think good".
 * Runs through all of their operations to make them
 * function when (an evil?) ninja comes into their world
****************************************************/
void enemy_ai() {
    for (int i = 0; i < enemymax; ++i) {
        enemy *e = &enemies[i];
        enemy_physics(e); // populates e->dir[]
        check_player_collision(e);

        if (!e->is_visible) { continue; } // off screen = switched off

        if (e->is_alive) {
            populate_dir(e);
            Boolean should_jump = FALSE;
            const Boolean moving_right = (player.x > e->x);  // always move towards the player
            const Boolean moving_left = !moving_right;
            e->x += moving_right ? e->speed : -e->speed;
            // flipped = moving right / not_flipped = moving left
            if (moving_left) {
                if (e->dir[4] || e->dir[1] == 7) { should_jump = TRUE; } // if there's a block (dir[4]) or lava (dir[1] == 7), try to jump it
                if (!e->dir[1] && !e->dir[7]) { should_jump = TRUE; } // if there's a drop (dir[1] == 0), and there's not a block blocking our jump (dir[7] == 0), try to jump it
            } else if (moving_right) { // moving right, if there's a block (dir[6]) or lava (dir[3] == 7), try to jump it
                if (e->dir[6] || e->dir[3] == 7) { should_jump = TRUE; } // if there's a block (dir[4]) or lava (dir[1] == 7), try to jump it
                if (!e->dir[3] && !e->dir[9]) { should_jump = TRUE; } // if there's a drop (dir[1] == 0), and there's not a block blocking our jump (dir[9] == 0), try to jump it
            }
            if (should_jump && !e->is_jumping) {
                e->gravity_compound = e->jump_strength;
                e->is_jumping = TRUE;
            }
        } else if (!e->is_alive && e->is_jumping) {
            e->x += e->is_flipped ? -5 : 5; // this gives dead enemies a knockback effect
            //const int x = wrldps.x + e->x + (e->w / 2);
            const int x = e->x + (e->w / 2);
            const int y = e->y + (e->h / 2);
            spawn_particle(blood1, x, y, 0, bounded_rand(0, 1), 1.0, bounded_rand(4, 8));
        }
    }
}

void draw_enemies(SDL_Surface *screen) {
    for (int i = 0; i < enemymax; ++i) {
        enemy *e = &enemies[i];
        update_animation_frame(e);
        draw_enemy(screen, e);
    }
}

inline bbox get_enemy_box(const enemy *e) {
    return (bbox) {
        .x = e->x,
        .y = e->y,
        .w = e->w,
        .h = e->h
    };
}

inline bbox get_enemy_attack_box(const enemy *e) {
    return (bbox) {
        .x = e->x - 10,
        .y = e->y,
        .w = 50,
        .h = 50
    };
}

