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

    if (sattack == 0 && score >= 5) {
        score -= 10; if(score < 0) { score = 0; }
        spdest.x = dest.x;
        spdest.y = dest.y + 15;
        attacklen = 25;
        if (ninja_src.x >= 180) { // which direction is the ninja facing...
            sattack = 1;
        } else {
            sattack = 2;
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
    if (attacklen == 0) {
        sattack = 0;
        return;
    }

    if (sattack == 1 || sattack == 2) { // attacking left (1) or right (2)
        if (sattack == 1) spdest.x += SPECIAL_ATTACK_SPEED;
        if (sattack == 2) spdest.x -= SPECIAL_ATTACK_SPEED;
        spdest.y += 1;
        SDL_BlitSurface((--attacklen % 2) ? sweapon1_1 : sweapon1_2, NULL, screen, &spdest);
    }
    killenemy();
}

void physics() {
    /*************************************************
     *ninja physisc
     *will not allow the ninja to go into boxes
     *will cause the ninja to fall if in the air
     *************************************************/
    if (dest.y > SCREENHEIGHT) {  dead(); return; } // Death by falling off the bottom of the screen

    /*checks in air collision left && right */
    // apply gravity
    gravity_compound += GRAVITY;
    if (gravity_compound > 15) { gravity_compound = 15; }
    if (gravity_compound < JUMPMAX) { gravity_compound = JUMPMAX; }
    dest.y += gravity_compound;

    // convert ninja position from screen position to world position
    const int ninja_world_x = dest.x + wrldps.x;

    int testY = dest.y + BRICK_HEIGHT - GRAVITY;
    int testX = ninja_world_x + BRICK_WIDTH - 10;
    int collision_type;

    if ((collision_type = isCollision(ninja_world_x + 10, testY)) || (collision_type = isCollision(testX, testY))) {
        if (collision_type == 7) { dead(); return; } // Tile 7 = up facing lava
        dest.y = (testY / BRICK_HEIGHT - 1) * BRICK_HEIGHT;
        jump = 0; gravity_compound = 0;
        ninja_src.y = 80; // set frame to on the ground
    } else if (isCollision(testX, dest.y) || isCollision(ninja_world_x, dest.y)) {/*will check bootom*/
        dest.y = (testY / BRICK_HEIGHT) * BRICK_HEIGHT;
        gravity_compound = 0;
    }

    testX = ninja_world_x + BRICK_WIDTH - 5;
    testY = dest.y + BRICK_HEIGHT - 10;

    if ((collision_type = isCollision(testX, dest.y)) || (collision_type = isCollision(testX, testY))) {
        if (collision_type == 6) { dead(); return; } // Tile 6 = left facing spikes
        dest.x -= MOVERL;
    } else if ((collision_type = isCollision(ninja_world_x, dest.y)) || (collision_type = isCollision(ninja_world_x, testY))) {
        if (collision_type == 8) {  dead(); return; } // Tile 8 = right facing spikes
        dest.x += MOVERL;
    }
}

// Checks for attack (sword) and special attack (star) kills
void killenemy() {
    if (!attack && !sattack) { return; } // not attacking or special attacking, so no chance to kill

    // adjust sword attack box based on the direction the player is facing
    const bbox attackBox = { dest.x + (ninja_src.x >= 180 ? 45 : 0), dest.y + 25, 20, 30 };
    const bbox sattackBox = { spdest.x, spdest.y, 30, 33 };

    for (int i = 0; i < enemymax; i++) {/*checks for each enemy*/
        if (nmy[i].nmy_alive == 0) { continue; } /*don't recheck the dead*/

        const bbox enemyBox = { nmy[i].nmydest.x, nmy[i].nmydest.y, 60, 80 };

        if (attack && bbox_col(enemyBox, attackBox)) { // Sword attack
            nmy[i].nmy_alive = 0;
            blood(nmy[i].nmydest);
            score += 10;
            nmy[i].nmy_deathtype = BYSWORD;
        } else if (sattack && bbox_col(enemyBox, sattackBox)) { // Star attack
            nmy[i].nmy_alive = 0;
            nmy[i].nmy_deathtype = BYSTAR;
            blood(nmy[i].nmydest);
            sattack = 0;
            // no more stars, and no sword out, don't bother checking anything else
            if (!attack) { return; }
        }
    }
}
