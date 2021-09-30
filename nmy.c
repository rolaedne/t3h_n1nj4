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
void killplayer(int i) {
    enemy *e = &nmy[i];
    if (isDead || !e->nmy_alive) { return; } // don't kill the player if they're already dead, and dead enemies can't hurt the player
    bbox player_box = { dest.x - 5, dest.y + 5, 50, 70 };
    bbox enemy_box = { e->nmydest.x - 10, e->nmydest.y, 50, 50 };
    if (bbox_col(player_box, enemy_box)) { dead(); }
}

void nmy_spwn(int i) {
    enemy *e = &nmy[i];
    const int right_edge = e->nmydest.x + (BRICK_WIDTH * 2);
    if ((e->nmydest.x - wrldps.x) < SCREENWIDTH && right_edge > 0) {/*if enemy is alive draw him on screen*/
        if (e->nmy_alive) {
            e->onscreen = 1;
            e->flipped = (e->nmydest.x < dest.x); // living enemies should face the player
            SDL_Surface *enemy_surface = e->flipped ? e->enemies_flipped[e->nmyani] : e->enemies[e->nmyani];
            SDL_BlitSurface(enemy_surface, NULL, screen, &e->nmydest);
        } else {
            SDL_Surface *death_surface = e->flipped ? e->deaths_flipped[e->nmy_deathtype] : e->deaths[e->nmy_deathtype];
            if (death_surface != NULL) {
                e->onscreen = 1;
                if (e->nmy_death_counter > 0 || rand() % 75 == 0) {
                    blood(e->nmydest);
                    e->nmy_death_counter--;
                }
                const int old_x = e->nmydest.x; // drawing off the edge of a surface (screen) will clip the dest rect
                const int old_y = e->nmydest.y; // TODO: x, y shouldn't be in an SDL_Rect directly used to draw
                SDL_BlitSurface(death_surface, NULL, screen, &e->nmydest);
                e->nmydest.x = old_x;
                e->nmydest.y = old_y;
            }
        }
    }
}

void nmy_physics(int i) {
    /*****************************************
     *enemy physisc loop
     *collision detection for the enemies
     *return vals used to operat the enemy ai funct
     ******************************************/
    int tmp;

    nmy[i].nmydest.y += GRAVITY + nmy[i].enemy_gravity_compound;
    nmy[i].enemy_gravity_compound += GRAVITY;

    nmy[i].dir[5] = 1;

    if (nmy[i].nmydest.y + (BRICK_HEIGHT / 2) > SCREENHEIGHT) {
        nmy[i].nmy_alive = 0;
        blood(nmy[i].nmydest);
    }

    if (world[(nmy[i].nmydest.y + BRICK_HEIGHT - GRAVITY) / BRICK_HEIGHT]
            [(nmy[i].nmydest.x + 10 + wrldps.x) / BRICK_WIDTH] > 0) {/*checks up and down collision*/
        if (nmy[i].nmy_alive != 0 &&
                world[(nmy[i].nmydest.y + BRICK_HEIGHT - GRAVITY) / BRICK_HEIGHT]
                [(nmy[i].nmydest.x + 10 + wrldps.x) / BRICK_WIDTH] == 7) {/*lava check*/
            //blood(nmy[i].nmydest);
            //nmy[i].nmy_alive=0;
            nmy[i].dir[0] = 1;
        } else {
            tmp = (((nmy[i].nmydest.y + (BRICK_HEIGHT)) - GRAVITY) / BRICK_HEIGHT) - 1;
            tmp *= BRICK_HEIGHT;
            nmy[i].enemy_gravity_compound = 0;
            nmy[i].nmydest.y = tmp;
            nmy[i].dir[1] = 1;
            nmy[i].jmpon = 0;
        }
    }
    if (world[(nmy[i].nmydest.y + BRICK_HEIGHT - GRAVITY) / BRICK_HEIGHT]
            [(nmy[i].nmydest.x + BRICK_WIDTH - 5 + wrldps.x) / BRICK_WIDTH] > 0) {/*checks bottom right*/
        if (world[(nmy[i].nmydest.y + BRICK_HEIGHT - GRAVITY) / BRICK_HEIGHT]
                [(nmy[i].nmydest.x + BRICK_WIDTH - 10 + wrldps.x) / BRICK_WIDTH] == 7
                && nmy[i].nmy_alive != 0) {/*lava block collision equals death*/
            nmy[i].dir[0] = 1;
        } else {
            tmp = (((nmy[i].nmydest.y + (BRICK_HEIGHT)) - GRAVITY) / BRICK_HEIGHT) - 1;
            tmp *= BRICK_HEIGHT;
            nmy[i].enemy_gravity_compound = 0;
            nmy[i].nmydest.y = tmp;
            nmy[i].dir[3] = 1;
            nmy[i].jmpon = 0;
        }
    }
    if (world[(nmy[i].nmydest.y) / BRICK_HEIGHT]
            [(nmy[i].nmydest.x + BRICK_WIDTH - 10 + wrldps.x) / BRICK_WIDTH] > 0) {/*check top*/
        tmp = (((nmy[i].nmydest.y + (BRICK_HEIGHT)) - GRAVITY) / BRICK_HEIGHT) - 1;
        tmp *= BRICK_HEIGHT;
        nmy[i].enemy_gravity_compound = 0;
        nmy[i].nmydest.y = tmp + BRICK_HEIGHT;
        nmy[i].dir[8] = 1;
    }
    if (world[(nmy[i].nmydest.y) / BRICK_HEIGHT]
            [(nmy[i].nmydest.x + wrldps.x) / BRICK_WIDTH] > 0) {/*check top*/
        tmp = (((nmy[i].nmydest.y + (BRICK_HEIGHT)) - GRAVITY) / BRICK_HEIGHT) - 1;
        tmp *= BRICK_HEIGHT;
        nmy[i].enemy_gravity_compound = 0;
        nmy[i].nmydest.y = tmp + BRICK_HEIGHT;
        nmy[i].dir[8] = 1;
    }

    if (world[nmy[i].nmydest.y / BRICK_HEIGHT]
            [(nmy[i].nmydest.x + BRICK_WIDTH - 10 + wrldps.x) / BRICK_WIDTH] > 0) {/*checks side to side block collision*/
        nmy[i].nmydest.x -= MOVERL;
        nmy[i].dir[9] = 1;
    }
    if (world[nmy[i].nmydest.y / BRICK_HEIGHT][(nmy[i].nmydest.x + wrldps.x) / BRICK_WIDTH] > 0) {
        nmy[i].nmydest.x += MOVERL;
        nmy[i].dir[7] = 1;
    }
    if (world[(nmy[i].nmydest.y + 70) / BRICK_HEIGHT]
            [(nmy[i].nmydest.x + BRICK_WIDTH - 1 + wrldps.x) / BRICK_WIDTH] > 0) {
        nmy[i].nmydest.x -= MOVERL;
        nmy[i].dir[6] = 1;
    }
    if (world[(nmy[i].nmydest.y + 70) / BRICK_HEIGHT][(nmy[i].nmydest.x + wrldps.x) / BRICK_WIDTH] > 0) {
        nmy[i].nmydest.x += MOVERL;
        nmy[i].dir[4] = 1;
    }
    if (world[(nmy[i].nmydest.y + BRICK_HEIGHT - 20) / BRICK_HEIGHT]
            [(nmy[i].nmydest.x + (BRICK_WIDTH / 2) + wrldps.x) / BRICK_WIDTH] == 7
            && nmy[i].nmy_alive != 0) {/*fallen into lava pit*/
        nmy[i].nmy_alive = 0;
        nmy[i].nmy_deathtype = BYLAVA;

        blood(nmy[i].nmydest);
    }
}/*end nmy physcis*/

void enemyai() {
    /***************************************************
     *This is the brain of the enemy. This makes them "think good".
     *Runs through all of their operations to make them
     *function when (an evil?) ninja comes into their world
     ****************************************************/
    int i, t;

    for (i = 0; i < enemymax; i++) {
        enemy *e = &nmy[i];
        nmy_physics(i);
        nmy_spwn(i);
        killplayer(i);
        enemyanimation(i);

        if (nmy[i].onscreen == 1 && nmy[i].nmy_alive == 1) {/*is the enemy on the screen*/
            if ((nmy[i].dir[4] || nmy[i].dir[7]) && nmy[i].jmpon == 0) {/*block on the left*/
                nmy[i].enemy_gravity_compound = nmy[i].jmp;
                nmy[i].jmpon = 1;
            }
            if ((nmy[i].dir[9] || nmy[i].dir[6])
                    && nmy[i].jmpon == 0) {/*block on the right*/
                nmy[i].enemy_gravity_compound = nmy[i].jmp;
                nmy[i].jmpon = 1;
            }
            if (nmy[i].dir[3] == 1 || nmy[i].dir[1] == 1 || nmy[i].dir[5] == 1) {/*on the ground or in the air go toward ninja*/
                if (dest.x > nmy[i].nmydest.x) {
                    nmy[i].nmydest.x += nmy[i].speed;
                    /*add check for other enemies*/
                } else {
                    nmy[i].nmydest.x -= nmy[i].speed;
                }
                if ((nmy[i].dir[3] == 1 || nmy[i].dir[0] == 1) && nmy[i].dir[1] == 0
                        && nmy[i].jmpon == 0) {/*a pit is ahead* left*/
                    nmy[i].enemy_gravity_compound = nmy[i].jmp;
                    nmy[i].jmpon = 1;
                } else if ((nmy[i].dir[3] == 0 || nmy[i].dir[0] == 1) && nmy[i].dir[1] == 0
                        && nmy[i].jmpon == 0) {/*a pit is ahead* left*/
                    nmy[i].enemy_gravity_compound = nmy[i].jmp;
                    nmy[i].jmpon = 1;
                }
            }
        } else if (nmy[i].onscreen == 1 && nmy[i].nmy_alive == 0 && nmy[i].jmpon == 1) {
            /* this gives dead enemies a knockback effect */
            if (ninja_src.x < 180) {
                nmy[i].nmydest.x -= 5; /*temp val*/
            } else if (ninja_src.x >= 180) {
                nmy[i].nmydest.x += 5; /*temp val*/
            }
            int x, y;
            x = nmy[i].nmydest.x + nmy[i].nmydest.w / 2;
            y = nmy[i].nmydest.y + nmy[i].nmydest.h / 2;
            x += wrldps.x;
            addParticle(blood1, x, y, 0, 0, 1.0, 7);
        }
        for (t = 0; t < 10; t++) {/*rests their dir arrray*/
            nmy[i].dir[t] = 0;
        }
    }/*end for loop*/
}

void enemyanimation(int i) {
    enemy *e = &nmy[i];
    if (e->nmydly-- <= 0) {
        e->nmydly = NMYDLY;
        e->nmyani++;
        if (e->nmyani >= NMY_FRAMES || e->enemies[e->nmyani] == NULL) { e->nmyani = 0; }
    }
}
