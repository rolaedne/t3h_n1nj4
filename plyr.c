/*******************************************
 *Roderick Newill
 *plr.c
 *This handles all things ninja
 *where he can hit with special weapon(s) and his sword.
 *collision with the world.
 ********************************************/

#include <SDL2/SDL.h>
#include <math.h>

#include "world.h"
#include "nmy.h"
#include "plyr.h"
#include "image.h"
#include "utils.h"
#include "screens.h"
#include "controls.h"

void check_for_kills();

void player_physics() {
    /*************************************************
     *ninja physisc
     *will not allow the ninja to go into boxes
     *will cause the ninja to fall if in the air
     *************************************************/
    if (player.y > WORLD_ROWS * BRICK_HEIGHT) {  dead(); return; } // Death by falling off the bottom of the world

    /*checks in air collision left && right */
    // apply gravity
    player.gravity_compound += GRAVITY;
    if (player.gravity_compound > 15) { player.gravity_compound = 15; }
    if (player.gravity_compound < JUMPMAX) { player.gravity_compound = JUMPMAX; }
    player.y += player.gravity_compound;

    // TODO: player.x is now the same as ninja_world_x, so we can get rid of this.
    const int ninja_world_x = player.x;

    int test_y = player.y + BRICK_HEIGHT - GRAVITY;
    int test_x = ninja_world_x + BRICK_WIDTH - 10;
    int collision_type;
    int collision_type2;
    collision_type = is_collision(ninja_world_x + 10, test_y); // left foot
    collision_type2 = is_collision(test_x, test_y); // right foot
    if (collision_type || collision_type2) {
        if (collision_type == 7 && (collision_type2 == 7 || collision_type2 == 0)) { dead(); return; } // Tile 7 = up facing lava
        if (collision_type == 0 && collision_type2 == 7) { dead(); return; } // Tile 7 = up facing lava
        player.y = (test_y / BRICK_HEIGHT - 1) * BRICK_HEIGHT;
        player.is_jumping = FALSE;
        player.gravity_compound = 0;
    } else if (is_collision(test_x, player.y) || is_collision(ninja_world_x, player.y)) {/*will check bootom*/
        player.y = (test_y / BRICK_HEIGHT) * BRICK_HEIGHT;
        player.gravity_compound = 0;
    }

    test_x = ninja_world_x + BRICK_WIDTH - 5;
    test_y = player.y + BRICK_HEIGHT - 10;

    if ((collision_type = is_collision(test_x, player.y)) || (collision_type = is_collision(test_x, test_y))) {
        if (collision_type == 7) { return; } // Tile 7 = lava, if you're in it, stay in it
        if (collision_type == 6) { dead(); return; } // Tile 6 = left facing spikes
        player.x -= MOVERL;
    } else if ((collision_type = is_collision(ninja_world_x, player.y)) || (collision_type = is_collision(ninja_world_x, test_y))) {
        if (collision_type == 7) { return; } // Tile 7 = lava, if you're in it, stay in it
        if (collision_type == 8) {  dead(); return; } // Tile 8 = right facing spikes
        player.x += MOVERL;
    }

    if (player.attack > 0) {
        player.attack--;
    }
    check_for_kills();
}

// Checks for attack (sword) and special attack (star) kills
void check_for_kills() {
    if (player.is_dead || (!player.attack)) { return; } // not attacking, so no chance to kill

    // adjust sword attack box based on the direction the player is facing
    const bbox attackBox = { player.x + (player.is_facing_right ? 45 : 0), player.y + 25, 20, 30 };

    for (int i = 0; i < NMY; ++i) {
        enemy *e = &enemies[i];
        if (e->is_alive == 0) { continue; } // don't recheck the dead

        const bbox enemyBox = { e->x, e->y, e->w, e->h };

        if (bbox_collision(enemyBox, attackBox)) { // Sword attack
            e->is_alive = 0;
            player.score += 10;
            e->death_type = BYSWORD;
            e->death_bleed_counter = 25; // how bloody is the death?
        }
    }
}


// ninja_src is references a 360w x 240h (6 x 3) sprite sheet
// the ninja is 60w x 80h
// the left half is left facing, the right half is right facing
// the top row is jumping (nothing, sword out (left), sword sheathed (left), sword sheathed (right), sword out (right), nothing)
// the middle row is "neutral" (run-left-1, run-left-2, standing-left, standing-right, run-right-2, run-right-1)
// the bottom row is sword-drawn (run-left-1, run-left-2, standing-left, standing-right, run-right-2, run-right-1)

int animation_frame = 0;

int get_player_frame_y() {
    if (player.is_dead || player.is_jumping) { return 0; }
    if (player.is_attacking) { return 160; } // player.h * 2
    return 80; // on ground, not attacking
}

int get_player_frame_x() {
    if (player.is_dead) { return player.is_facing_right ? 300 : 0; }
    if (player.is_jumping) {
        if (player.is_attacking) { return player.is_facing_right ? 240 : 60; }
        return player.is_facing_right ? 180 : 120;
    }
    if (!player.is_running) { return player.is_facing_right ? 180 : 120; }
    animation_frame = !animation_frame;
    const int offset = animation_frame ? 60 : 0;
    return player.is_facing_right ? 240 + offset : 0 + offset;
}

void draw_player() {
    player.is_attacking = player.attack > 0;
    SDL_Rect src = { get_player_frame_x(), get_player_frame_y(), player.w, player.h };
    SDL_Rect dest = { player.x - vp.x, player.y - vp.y, player.w, player.h };
    SDL_BlitSurface(player.ninja, &src, screen, &dest);
}
