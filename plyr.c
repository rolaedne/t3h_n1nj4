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
        score -= 10;
        if(score < 0) { score = 0; }
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
    }

    if (sattack == 1 || sattack == 2) { // attacking left (1) or right (2)
        if (sattack == 1) spdest.x += SPECIAL_ATTACK_SPEED;
        if (sattack == 2) spdest.x -= SPECIAL_ATTACK_SPEED;
        spdest.y += 1;
        attacklen--;
        if ((attacklen % 2) == 0) {
            SDL_BlitSurface(sweapon1_1, NULL, screen, &spdest);
        } else {
            SDL_BlitSurface(sweapon1_2, NULL, screen, &spdest);
        }
    }
    killenemy();
}

void physics() {
    /*************************************************
     *ninja physisc
     *will not allow the ninja to go into boxes
     *will cause the ninja to fall if in the air
     *************************************************/
    int tmp;
    if (dest.y > SCREENHEIGHT) {
        blood(dest);
        dead();
    }
    /*checks in air collision left && right */
    dest.y += GRAVITY + gravity_compound;
    gravity_compound += GRAVITY;
    if(gravity_compound > 15) gravity_compound = 15;
    if(gravity_compound < JUMPMAX) gravity_compound = JUMPMAX;

    if (world[(dest.y + BRICK_HEIGHT) / BRICK_HEIGHT][(dest.x + wrldps.x + 30) / BRICK_WIDTH] == 7) {
        blood(dest);
        dead();
    }

    if (world[(dest.y + BRICK_HEIGHT - GRAVITY) / BRICK_HEIGHT][(dest.x + 10 + wrldps.x) / BRICK_WIDTH] > 0
            && world[(dest.y + BRICK_HEIGHT - GRAVITY) / BRICK_HEIGHT][(dest.x + 10 + wrldps.x) / BRICK_WIDTH] != 7) {//check fall left side
        tmp = (((dest.y + (BRICK_HEIGHT)) - GRAVITY) / BRICK_HEIGHT) - 1;
        tmp *= BRICK_HEIGHT;
        gravity_compound = 0;
        dest.y = tmp;
        jump = 0;
        ninja_src.y = 80;
    } else if (world[(dest.y + BRICK_HEIGHT - GRAVITY) / BRICK_HEIGHT]
            [(dest.x + BRICK_WIDTH - 10 + wrldps.x) / BRICK_WIDTH] > 0 &&
            world[(dest.y + BRICK_HEIGHT - GRAVITY) / BRICK_HEIGHT][(dest.x + BRICK_WIDTH - 10 + wrldps.x) / BRICK_WIDTH] != 7) {//check fall right side
        tmp = (((dest.y + (BRICK_HEIGHT)) - GRAVITY) / BRICK_HEIGHT) - 1;
        tmp *= BRICK_HEIGHT;
        gravity_compound = 0;
        dest.y = tmp;
        jump = 0;
        ninja_src.y = 80;
    } else if (world[(dest.y) / BRICK_HEIGHT][(dest.x + BRICK_WIDTH - 10 + wrldps.x) / BRICK_WIDTH] > 0) {/*will check bootom*/
        tmp = (((dest.y + (BRICK_HEIGHT)) - GRAVITY) / BRICK_HEIGHT) - 1;
        tmp *= BRICK_HEIGHT;
        gravity_compound = 0;
        dest.y = tmp + BRICK_HEIGHT;
    } else if (world[(dest.y) / BRICK_HEIGHT][(dest.x + wrldps.x) / BRICK_WIDTH] > 0) {/*will check bootom pther side*/
        tmp = (((dest.y + (BRICK_HEIGHT)) - GRAVITY) / BRICK_HEIGHT) - 1;
        tmp *= BRICK_HEIGHT;
        gravity_compound = 0;
        dest.y = tmp + BRICK_HEIGHT;
    }

    /*checks on ground left && right */
    if (world[dest.y / BRICK_HEIGHT][(dest.x + BRICK_WIDTH - 5 + wrldps.x) / BRICK_WIDTH] > 0) {
        if (world[dest.y / BRICK_HEIGHT][(dest.x + BRICK_WIDTH - 5 + wrldps.x) / BRICK_WIDTH] == 6) {
            dead();
        }
        dest.x -= MOVERL;
    } else if (world[dest.y / BRICK_HEIGHT][(dest.x + wrldps.x) / BRICK_WIDTH] > 0) {
        if (world[dest.y / BRICK_HEIGHT][(dest.x + wrldps.x) / BRICK_WIDTH] == 8) {
            dead();
        }
        dest.x += MOVERL;
    } else if (world[(dest.y + 70) / BRICK_HEIGHT][(dest.x + BRICK_WIDTH - 5 + wrldps.x) / BRICK_WIDTH] > 0) {
        if (world[(dest.y + 70) / BRICK_HEIGHT][(dest.x + BRICK_WIDTH - 5 + wrldps.x) / BRICK_WIDTH] == 6) {
            dead();
        }
        dest.x -= MOVERL;
    } else if (world[(dest.y + 70) / BRICK_HEIGHT][(dest.x + wrldps.x) / BRICK_WIDTH] > 0) {
        if (world[(dest.y + 70) / BRICK_HEIGHT][(dest.x + wrldps.x) / BRICK_WIDTH] == 8) {
            dead();
        }
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
