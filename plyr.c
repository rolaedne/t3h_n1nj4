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
        special_throw();
        attacklen = 25;
        if (ninja_src.x >= 180) {
            sattack = 1;
        } else {
            sattack = 2;
        }
    }
}

#define SPECIAL_ATTACK_SPEED 25

void special_throw() {
    /*************************************************
     *displays special weapon
     *will cause the special weapon to be displayed on
     *the screen & determin if it kills the enemy
     *************************************************/
    if (attacklen == 0) {
        sattack = 0;
    }

    if (sattack == 1) {
        spdest.x += SPECIAL_ATTACK_SPEED;
        spdest.y += 1;
        attacklen--;
        if ((attacklen % 5) == 0) {
            SDL_BlitSurface(sweapon1_1, NULL, screen, &spdest);
        } else {
            SDL_BlitSurface(sweapon1_2, NULL, screen, &spdest);
        }
    } else if (sattack == 2) {
        spdest.x -= SPECIAL_ATTACK_SPEED;
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

void winner() {
    printf("DEBUG: winner\n");
    /******************************************
     *will have the winning stuff
     *******************************************/
    SDL_Surface *winnerimg = loadImageAsSurface("lvl/winner.png");
    SDL_BlitSurface(winnerimg, NULL, screen, NULL);
    SDL_Flip(screen);
    SDL_PumpEvents();
    SDL_Delay(60 * 45);
    SDL_FreeSurface(winnerimg);
    winnerimg = NULL;
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

void dead() {
    /******************************
     *will be dead charecter screen
     *******************************/
    deaths += 1;
    SDL_Surface *death_screen;
    BOOLEAN done = FALSE;

    SDL_BlitSurface(blood1, NULL, screen, &dest);
    SDL_Flip(screen);
    SDL_Delay(1000);

    death_screen = loadImageAsSurface("lvl/dead.png");
    SDL_BlitSurface(death_screen, NULL, screen, NULL); /*print dead screen*/
    SDL_Flip(screen);
    SDL_FreeSurface(death_screen);
    death_screen = NULL;


    while (done == FALSE && SDL_WaitEvent(&event) != -1) {
        switch (event.type) {
            case SDL_QUIT: /*quits program*/
                exit(0);
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        exit(EXIT_SUCCESS);
                        /*replace w/ an exitgame function */
                        break;
                    case SDLK_SPACE:
                        done = TRUE;
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    score -= 10;
    if (score < 0) score = 0;
    sattack = 0;
    attack = 0;
    buildw();
}

void killenemy() {
    /*****************************
     *kills enemy if hit by weapon
     *****************************/
    int i;
    for (i = 0; i < enemymax; i++) {/*checks for each enemy*/
        if (nmy[i].nmy_alive == 0) continue; /*don't recheck the dead*/
        if (attack != 0) {/*checks for if in attack mode*/
            if (ninja_src.x >= 180) {/*setting destionat of sword in image with first 4 vals*/
                if (twoblock_col(nmy[i].nmydest.x, nmy[i].nmydest.y, 60, 80,
                        dest.x + 45, dest.y + 25, 20, 30)
                        == 1 && nmy[i].nmy_alive != 0) {
                    nmy[i].nmy_alive = 0;
                    blood(nmy[i].nmydest);
                    score += 10;
                }
            } else {/*setting destionation of sword in image with first 4 vals*/
                if (twoblock_col(nmy[i].nmydest.x, nmy[i].nmydest.y, 60, 80,
                        dest.x, dest.y + 25, 20, 30)
                        == 1 && nmy[i].nmy_alive != 0) {
                    nmy[i].nmy_alive = 0;
                    blood(nmy[i].nmydest);
                    score += 10;
                }
            }
            if (nmy[i].nmy_alive == 0) {
                nmy[i].nmy_deathtype = BYSWORD;
            }
        }
        if (sattack != 0 &&
                /*special attack (star)*/
                twoblock_col(nmy[i].nmydest.x, nmy[i].nmydest.y, 60, 80,
                spdest.x, spdest.y, 30, 33) == 1 && nmy[i].nmy_alive != 0) {/*special weapon attacks*/
            nmy[i].nmy_alive = 0;
            nmy[i].nmy_deathtype = BYSTAR;
            blood(nmy[i].nmydest);
            sattack = 0;
        }
    }
}
