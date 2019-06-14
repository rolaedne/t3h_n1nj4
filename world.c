/*******************************************
 *Roderick Newill
 *world.c
 *This tells how to build the world
 *worlds are constructed from files located in
 *lvl_files. These are stored in plain text.
 *also handles moveing the world when ninja moves
 *to far on the screen
 ********************************************/
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "world.h"
#include "nmy.h"
#include "plyr.h"
#include "particles.h"
#include "image.h"

int isCollision(int x, int y) {
    int row, col;
    row = y / BRICK_HEIGHT;
    col = x / BRICK_WIDTH;
    //printf("BRICK: %dx%d\n", row, col);
    if (row > WORLD_ROWS || row < 0) { return 1; }
    if (col > WORLD_COLS || col < 0) { return 1; }
    return world[row][col] > 0;
}

void graphics_load() {
    printf("DEBUG: graphics_load()\n");
    /*TODO: load checks on these*/
    /*init images*/
    foreground = NULL; //loadImageAsSurface("lvl/foreground0.png");
    wscore = loadImageAsSurface("lvl/score.png");
    number = loadImageAsSurface("lvl/number.png");
    blood1 = loadImageAsSurface("bloods/blood4.png");
    snow = loadImageAsSurface("bloods/snow.png");
    ash = loadImageAsSurface("bloods/ash.png");
    sweapon1_1 = loadImageAsSurface("weapons/star1.png");
    sweapon1_2 = loadImageAsSurface("weapons/star2.png");

    /*inits some world images*/
    ninja = loadImageAsSurface("chars/ninja.new2.png");

    worldfloor = loadImageAsSurface("lvl/floor.png");
    worldbrick[0] = loadImageAsSurface("lvl/brick0.png");
    worldbrick[1] = loadImageAsSurface("lvl/brick1.png");
    worldbrick[2] = loadImageAsSurface("lvl/brick2.png");
    worldbrick[3] = loadImageAsSurface("lvl/brick3.png");
    dmgbrick[0] = loadImageAsSurface("lvl/dbrick0.png");
    dmgbrick[1] = loadImageAsSurface("lvl/dbrick1.png");
    dmgbrick[2] = loadImageAsSurface("lvl/dbrick2.png");
}


void graphics_free() {
    int i;

    printf("DEBUG: graphics_free()\n");
    if(foreground != NULL) {
        SDL_FreeSurface(foreground);
        foreground = NULL;
    }
    if(wscore != NULL) {
        SDL_FreeSurface(wscore);
        wscore = NULL;
    }
    if(number != NULL) {
        SDL_FreeSurface(number);
        number = NULL;
    }
    if(blood1 != NULL) {
        SDL_FreeSurface(blood1);
        blood1 = NULL;
    }
    if(sweapon1_1 != NULL) {
        SDL_FreeSurface(sweapon1_1);
        sweapon1_2 = NULL;
    }
    if(sweapon1_2 != NULL) {
        SDL_FreeSurface(sweapon1_2);
        sweapon1_2 = NULL;
    }
    if(ninja != NULL) {
        SDL_FreeSurface(ninja);
        ninja = NULL;
    }
    if(worldfloor != NULL) {
        SDL_FreeSurface(worldfloor);
        worldfloor = NULL;
    }

    for(i = 0; i < BRICKS_WORLD; ++i) {
        if(worldbrick[i] != NULL) {
            SDL_FreeSurface(worldbrick[i]);
            worldbrick[i] = NULL;
        }
    }

    for(i = 0; i < BRICKS_DAMAGE; ++i) {
        if(dmgbrick[i] != NULL) {
            SDL_FreeSurface(dmgbrick[i]);
            dmgbrick[i] = NULL;
        }
    }

}


void buildw() {
    /***********************************************
     *creats the world(s)
     *reads in file that will constuct the world
     *file consits of:
    number of enemies
     *flollewed by enemy cooridinates x & y
     *leght of world
     *then world layout using the below world format
     ***************************************************/
    int i, t, x;
    char lvlname[50];
    FILE* world_file;

    printf("DEBUG: buildw()\n");
    /* free the memory used by world, if any*/
    if(background != NULL) {
        SDL_FreeSurface(background);
        background = NULL;
    }

    for(i = 0; i< NMY; ++i) {
        nmy[i].onscreen = 0;
        for(x = 0; x < NMY_FRAMES; ++x) {
            if(nmy[i].enemies[x] != NULL) {
                SDL_FreeSurface(nmy[i].enemies[x]);
                nmy[i].enemies[x] = NULL;
            }
        }
        for(x = 0; x < NMY_DEATHS; ++x) {
            if(nmy[i].deaths[x] != NULL) {
                SDL_FreeSurface(nmy[i].deaths[x]);
                nmy[i].deaths[x] = NULL;
            }
        }
    }



    /********************************************
     *world stuff needed to make world
     *0 nothing
     *1floor
     *2brick
     *********************************************/

    /*initialises game world to all zeros*/
    for (i = 0; i < WORLD_COLS; i++) {
        for (t = 0; t < WORLD_ROWS; t++) {
            world[t][i] = 0;
        }
    }
    /*inits wolrd image*/
    wrldps.x = 0;
    wrldps.y = 0;
    wrldps.w = 640;
    wrldps.h = 480;

    /*inits ninja image*/
    ninja_src.y = 80;
    ninja_src.x = 180;
    ninja_src.w = 60;
    ninja_src.h = 80;


    /*ninja start positison*/
    dest.x = 0;
    dest.y = 80 * 4;

    /*initialises alive loop for enemies*/
    for (i = 0; i < NMY; i++) {
        nmy[i].nmy_alive = 0;
    }

    /*world file to be used*/

    sprintf(lvlname, "lvl_files/lvl%d.txt", worldnum);
    world_file = fopen(lvlname, "r");

    if (world_file == NULL) {/*no more worlds to be read in*/
        winner();
        printf("winner was reached\n");
        graphics_free();
        exit(EXIT_SUCCESS);
    }

    fscanf(world_file, "%d", &x);

    enemymax = x; /*set max amount of enemies*/
    for (i = 0; i < NMY && i < enemymax; i++) {/*set enemy destinations from file*/
        fscanf(world_file, "%d", &x);
        nmy[i].nmytype = x;
        fscanf(world_file, "%d", &x);
        nmy[i].nmydest.x = x*BRICK_WIDTH;
        fscanf(world_file, "%d", &x);
        nmy[i].nmydest.y = x*BRICK_HEIGHT;
        nmy[i].nmy_alive = 1;
        if (nmy[i].nmytype == 0) {/*enemy type 0s quality*/
            nmy[i].enemies[0] = loadImageAsSurface("chars/turco0.png");
            nmy[i].enemies[1] = loadImageAsSurface("chars/turco1.png");

            /*death image, sword,*/
            nmy[i].deaths[BYSWORD] = loadImageAsSurface("chars/turco2.png");
            nmy[i].deaths[BYSTAR] = loadImageAsSurface("chars/turco3.png");
            nmy[i].deaths[BYLAVA] = loadImageAsSurface("chars/turco4.png");

            nmy[i].nmyani = 0; /*what animation frame to start on*/
            nmy[i].nmyanilen = 1; /*home many animations that is zero based*/
            nmy[i].jmp = -40;
            nmy[i].speed = 4;
            nmy[i].jmpon = 0;
        } else if (nmy[i].nmytype == 1) {
            nmy[i].enemies[0] = loadImageAsSurface("chars/one0.png");
            nmy[i].enemies[1] = loadImageAsSurface("chars/one1.png");
            nmy[i].enemies[2] = loadImageAsSurface("chars/one2.png");
            nmy[i].enemies[3] = loadImageAsSurface("chars/one3.png");

            nmy[i].nmyani = 0; /*what animation frame to start on*/
            nmy[i].nmyanilen = 3; /*home many animations that is zero based*/
            nmy[i].jmp = -45;
            nmy[i].speed = 5;
            nmy[i].jmpon = 0;
        } else if (nmy[i].nmytype == 2) {
            nmy[i].enemies[0] = loadImageAsSurface("chars/rninja0.png");
            nmy[i].enemies[1] = loadImageAsSurface("chars/rninja1.png");
            nmy[i].enemies[2] = loadImageAsSurface("chars/rninja2.png");
            nmy[i].enemies[3] = loadImageAsSurface("chars/rninja3.png");
            nmy[i].enemies[4] = loadImageAsSurface("chars/rninja4.png");
            nmy[i].enemies[5] = loadImageAsSurface("chars/rninja5.png");

            nmy[i].nmyani = 0; /*what animation frame to start on*/
            nmy[i].nmyanilen = 4; /*home many animations that is zero based*/
            nmy[i].jmp = -55;
            nmy[i].speed = 8;
            nmy[i].jmpon = 0;
        }
    }

    nmy[i].nmydly = 0; /*used to count in a delay funt*/
    nmy[i].onscreen = 0;
    i = 0;
    t = 5;

    fscanf(world_file, "%d", &x);
    world_length = x;

    //sprintf(lvlname, "lvl/background%d.png", worldnum);
    //background = loadImageAsSurface(lvlname);
    background = createSurface(x * BRICK_WIDTH, SCREENHEIGHT);
    SDL_Rect dst; dst.x = 0; dst.y = 0; dst.w = background->w; dst.h = background->h;

    SDL_FillRect(background, &dst, SDL_MapRGB(background->format, 0, 0, 0)); // black

    int bandHeight = 10 + deaths + (5 * worldnum);
    if (bandHeight > 25) bandHeight = 25;

    int backgroundHeight = background->h;
    int bands = backgroundHeight / bandHeight;

    int startColor = 64 + (deaths * 2) + (5 * worldnum);
    if (startColor > 128) startColor = 128;

    int maxBaseColor = 255;
    int colorStep = (maxBaseColor - startColor) / bands;

    dst.h = bandHeight;
    for (int i = 0; i < bands; i++) {
        dst.y = i * bandHeight;
        int baseColor = startColor + (i * colorStep);
        if (baseColor > maxBaseColor) baseColor = maxBaseColor;
        SDL_FillRect(background, &dst, SDL_MapRGB(background->format, baseColor / 2 + worldnum, baseColor / 2 + worldnum, baseColor));
    }

    //sprintf(lvlname, "lvl/foreground%d.png", worldnum);
    //foreground = NULL; //loadImageAsSurface(lvlname);

    while (!feof(world_file)) {/*creates game world from frile*/
        fscanf(world_file, "%d", &x);
        if (t == -1) {
            t = 5;
            i++;
        }
        world[t][i] = x;
        t--;
    }
    set_screen();
}

void set_screen() {
    int i, t;
    const int world_height = 6;
    /*******************************************
     *sets up the screen for score and the
     *game board to be displayed on the screen
     ********************************************/

    printf("DEBUG: set_screen()\n");
    SDL_BlitSurface(background, &wrldps, screen, NULL);

    for (i = 0; i < world_length; i++) {
        for (t = 0; t < world_height; t++) {
            if (world[t][i] == 1) {
                worlddest.y = t*BRICK_HEIGHT;
                worlddest.x = i*BRICK_WIDTH;
                SDL_BlitSurface(worldfloor, NULL, background, &worlddest);
                SDL_BlitSurface(worldfloor, NULL, screen, &worlddest);
            } else if (world[t][i] == 2) {
                worlddest.y = t*BRICK_HEIGHT;
                worlddest.x = i*BRICK_WIDTH;
                SDL_BlitSurface(worldbrick[0], NULL, background, &worlddest);
                SDL_BlitSurface(worldbrick[0], NULL, screen, &worlddest);
            } else if (world[t][i] == 3) {
                worlddest.y = t*BRICK_HEIGHT;
                worlddest.x = i*BRICK_WIDTH;
                SDL_BlitSurface(worldbrick[1], NULL, background, &worlddest);
                SDL_BlitSurface(worldbrick[1], NULL, screen, &worlddest);
            } else if (world[t][i] == 4) {
                worlddest.y = t*BRICK_HEIGHT;
                worlddest.x = i*BRICK_WIDTH;
                SDL_BlitSurface(worldbrick[2], NULL, background, &worlddest);
                SDL_BlitSurface(worldbrick[2], NULL, screen, &worlddest);
            } else if (world[t][i] == 5) {
                worlddest.y = t*BRICK_HEIGHT;
                worlddest.x = i*BRICK_WIDTH;
                SDL_BlitSurface(worldbrick[3], NULL, background, &worlddest);
                SDL_BlitSurface(worldbrick[3], NULL, screen, &worlddest);
            } else if (world[t][i] == 6) {
                worlddest.y = t*BRICK_HEIGHT;
                worlddest.x = i*BRICK_WIDTH;
                SDL_BlitSurface(dmgbrick[0], NULL, background, &worlddest);
                SDL_BlitSurface(dmgbrick[0], NULL, screen, &worlddest);
            } else if (world[t][i] == 7) {
                worlddest.y = t*BRICK_HEIGHT;
                worlddest.x = i*BRICK_WIDTH;
                SDL_BlitSurface(dmgbrick[1], NULL, background, &worlddest);
                SDL_BlitSurface(dmgbrick[1], NULL, screen, &worlddest);
            } else if (world[t][i] == 8) {
                worlddest.y = t*BRICK_HEIGHT;
                worlddest.x = i*BRICK_WIDTH;
                SDL_BlitSurface(dmgbrick[2], NULL, background, &worlddest);
                SDL_BlitSurface(dmgbrick[2], NULL, screen, &worlddest);
            }

        }
    }
}

void world_mover() {
    int i;
    /************************************************
     *  This function scrolls world if person is far
     *enought along
     *************************************************/
    if ((dest.x + wrldps.x + (SCREENWIDTH / 2))>(BRICK_WIDTH * world_length)) {/*can't scroll anymore to the right*/
        if ((dest.x + wrldps.x + 25)>(BRICK_WIDTH * world_length)) {/*reached the end of the screen*/
            worldnum++;
            buildw(); // Doesn't take worldnum as a parameter
        }
    } else if (dest.x > SCREENWIDTH / 2) {/*moves screen if past half screen lenght*/
        dest.x = SCREENWIDTH / 2;
        wrldps.x += MOVERL;
        for (i = 0; i < enemymax; i++) { nmy[i].nmydest.x -= MOVERL; }
    } else if (dest.x < 0) {/*can't run off the left side*/
        dest.x = 0;
    }
}

int twoblock_col(int bb1x, int bb1y, int bb1w, int bb1h,
        int bb2x, int bb2y, int bb2w, int bb2h) {
    /****************************************
     *checking for bounding block collisions
     *****************************************/

    /*check upper left corner of bb2*/
    if (bb1x <= bb2x && bb2x <= bb1x + bb1w)
        if (bb1y <= bb2y && bb2y <= bb1y + bb1h)
            return 1;

    /*check upper right corner of bb2*/
    if (bb1x <= bb2x + bb2w && bb2x + bb2w <= bb1x + bb1w)
        if (bb1y <= bb2y && bb2y <= bb1y + bb1h)
            return 1;

    /*check bottom left corner of bb2*/
    if (bb1x <= bb2x && bb2x <= bb1x + bb1w)
        if (bb1y <= bb2y + bb2h && bb2y + bb2h <= bb1y + bb1h)
            return 1;

    /*check bottom right corner of bb2*/
    if (bb1x <= bb2x + bb2w && bb2x + bb2w <= bb1x + bb1w)
        if (bb1y <= bb2y + bb2h && bb2y + bb2h <= bb1y + bb1h)
            return 1;
    return 0;
}

int bRand(int min, int max) {
    int random = rand() % max;
    if (random < min) {
        random += min;
    }
    return random;
}

void blood(SDL_Rect bleed) {
    /***********************************************************
     *causes blood to be placed on the screen
     *takes in teh sdl_rect to know where to place the blood
     *it takes into account the the lenght of the world
     *************************************************************/
    int x, y;
    x = bleed.x + bleed.w / 2;
    y = bleed.y + bleed.h / 2;
    //printf("DEBUG: blood(%d, %d)\n", x, y);
    x += wrldps.x;

    addParticle(blood1, x, y, bRand(1, 5), -1 * bRand(3, 10), 1.0);
    addParticle(blood1, x, y, bRand(0, 3), -1 * bRand(6, 20), 1.0);
    addParticle(blood1, x, y, bRand(1, 4) *-1, -1 * bRand(2, 8), 1.0);
    addParticle(blood1, x, y, bRand(1, 5), -1 * bRand(3, 10), 1.0);
    addParticle(blood1, x, y, bRand(0, 3), -1 * bRand(4, 12), 1.0);


    //SDL_BlitSurface(blood1, NULL, background, &bleed);
}

void rprint(int val) {
    /**************************************
     *prints digit to the screen
     *number is inited in fe.c
     ***************************************/
    SDL_Rect wdest, didest;

    //printf("DEBUG: rprint(%d)\n", val);
    didest.h = 13;
    didest.y = 0;
    didest.w = 10;
    wdest.x = 110;
    wdest.y = 0;

    SDL_BlitSurface(wscore, NULL, screen, NULL);

    if (val <= 0) {
        didest.x = 0;
        SDL_BlitSurface(number, &didest, screen, &wdest);
    } else {
        while (val > 0) {
            if ((val % 10) == 0) {
                didest.x = 0;
            } else if ((val % 10) == 1) {
                didest.x = 11;
            } else if ((val % 10) == 2) {
                didest.x = 22;
            } else if ((val % 10) == 3) {
                didest.x = 33;
            } else if ((val % 10) == 4) {
                didest.x = 44;
            } else if ((val % 10) == 5) {
                didest.x = 55;
            } else if ((val % 10) == 6) {
                didest.x = 66;
            } else if ((val % 10) == 7) {
                didest.x = 77;
            } else if ((val % 10) == 8) {
                didest.x = 88;
            } else if ((val % 10) == 9) {
                didest.x = 100;
            }
            SDL_BlitSurface(number, &didest, screen, &wdest);
            val = val / 10;
            wdest.x -= 10;
        }
    }
}
