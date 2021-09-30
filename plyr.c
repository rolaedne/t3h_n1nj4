/*******************************************
 *Roderick Newill
 *plr.c
 *This handles all things ninja
 *where he can hit with special weapon(s) and his sword.
 *collision with the world.
 ********************************************/

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "world.h"
#include "nmy.h"
#include "plyr.h"
#include "image.h"
#include "utils.h"
#include "screens.h"

void special() {
    /*************************************************
     *starts special weapon attack
     *this is called when the weapon special button is called
     *at this time that would be key press down
     **************************************************/

    if (player.sattack == 0 && player.score >= 5) {
        player.score -= 10; if(player.score < 0) { player.score = 0; }
        player.spdest.x = player.dest.x;
        player.spdest.y = player.dest.y + 15;
        player.attacklen = 25;
        if (player.ninja_src.x >= 180) { // which direction is the ninja facing...
            player.sattack = 1;
        } else {
            player.sattack = 2;
        }
    }
}

#define SPECIAL_ATTACK_SPEED 25

// TODO: Move this logic into an entity spawned by the the throw
void special_throw() {
    /*************************************************
     *displays special weapon
     *will cause the special weapon to be displayed on
     *the screen & determin if it kills the enemy
     *************************************************/
    if (player.attacklen == 0) {
        player.sattack = 0;
        return;
    }

    if (player.sattack == 1 || player.sattack == 2) { // attacking left (1) or right (2)
        if (player.sattack == 1) player.spdest.x += SPECIAL_ATTACK_SPEED;
        if (player.sattack == 2) player.spdest.x -= SPECIAL_ATTACK_SPEED;
        player.spdest.y += 1;
        SDL_BlitSurface((--player.attacklen % 2) ? player.sweapon1_1 : player.sweapon1_2, NULL, screen, &player.spdest);
    }
    killenemy();
}

void physics() {
    /*************************************************
     *ninja physisc
     *will not allow the ninja to go into boxes
     *will cause the ninja to fall if in the air
     *************************************************/
    if (player.dest.y > SCREENHEIGHT) {  dead(); return; } // Death by falling off the bottom of the screen

    /*checks in air collision left && right */
    // apply gravity
    player.gravity_compound += GRAVITY;
    if (player.gravity_compound > 15) { player.gravity_compound = 15; }
    if (player.gravity_compound < JUMPMAX) { player.gravity_compound = JUMPMAX; }
    player.dest.y += player.gravity_compound;

    // convert ninja position from screen position to world position
    const int ninja_world_x = player.dest.x + wrldps.x;

    int testY = player.dest.y + BRICK_HEIGHT - GRAVITY;
    int testX = ninja_world_x + BRICK_WIDTH - 10;
    int collision_type;
    int collision_type2;
    collision_type = is_collision(ninja_world_x + 10, testY); // left foot
    collision_type2 = is_collision(testX, testY); // right foot
    if (collision_type || collision_type2) {
        if (collision_type == 7 && (collision_type2 == 7 || collision_type2 == 0)) { dead(); return; } // Tile 7 = up facing lava
        if (collision_type == 0 && collision_type2 == 7) { dead(); return; } // Tile 7 = up facing lava
        player.dest.y = (testY / BRICK_HEIGHT - 1) * BRICK_HEIGHT;
        player.jump = 0; player.gravity_compound = 0;
        player.ninja_src.y = 80; // set frame to on the ground
    } else if (is_collision(testX, player.dest.y) || is_collision(ninja_world_x, player.dest.y)) {/*will check bootom*/
        player.dest.y = (testY / BRICK_HEIGHT) * BRICK_HEIGHT;
        player.gravity_compound = 0;
    }

    testX = ninja_world_x + BRICK_WIDTH - 5;
    testY = player.dest.y + BRICK_HEIGHT - 10;

    if ((collision_type = is_collision(testX, player.dest.y)) || (collision_type = is_collision(testX, testY))) {
        if (collision_type == 7) { return; } // Tile 7 = lava, if you're in it, stay in it
        if (collision_type == 6) { dead(); return; } // Tile 6 = left facing spikes
        player.dest.x -= MOVERL;
    } else if ((collision_type = is_collision(ninja_world_x, player.dest.y)) || (collision_type = is_collision(ninja_world_x, testY))) {
        if (collision_type == 7) { return; } // Tile 7 = lava, if you're in it, stay in it
        if (collision_type == 8) {  dead(); return; } // Tile 8 = right facing spikes
        player.dest.x += MOVERL;
    }
}

// Checks for attack (sword) and special attack (star) kills
void killenemy() {
    if (player.is_dead || (!player.attack && !player.sattack)) { return; } // not attacking or special attacking, so no chance to kill

    // adjust sword attack box based on the direction the player is facing
    const bbox attackBox = { player.dest.x + (player.ninja_src.x >= 180 ? 45 : 0), player.dest.y + 25, 20, 30 };
    const bbox sattackBox = { player.spdest.x, player.spdest.y, 30, 33 };

    for (int i = 0; i < enemymax; i++) {/*checks for each enemy*/
        if (enemies[i].is_alive == 0) { continue; } /*don't recheck the dead*/

        const bbox enemyBox = { enemies[i].dest.x, enemies[i].dest.y, 60, 80 };

        if (player.attack && bbox_collision(enemyBox, attackBox)) { // Sword attack
            enemies[i].is_alive = 0;
            player.score += 10;
            enemies[i].death_type = BYSWORD;
            enemies[i].death_bleed_counter = 25; // how bloody is the death?
        } else if (player.sattack && bbox_collision(enemyBox, sattackBox)) { // Star attack
            enemies[i].is_alive = 0;
            enemies[i].death_type = BYSTAR;
            enemies[i].death_bleed_counter = 10; // how bloody is the death?
            player.score += 5; // small refund for star kills
            player.sattack = 0;
            if (!player.attack) { return; } // no more stars, and no sword out, don't bother checking anything else
        }
    }
}
