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
#include "utils.h"
#include "screens.h"

#define BRICKS_WORLD 5
#define BRICKS_DAMAGE 5

SDL_Surface *dmgbrick[BRICKS_DAMAGE];
int world_length;
SDL_Surface *worldfloor, *worldbrick[BRICKS_WORLD];
SDL_Rect worlddest;
SDL_Surface *snow;

int is_collision(const int x, const int y) {
    const int row = y / BRICK_HEIGHT;
    if (row > WORLD_ROWS || row < 0) { return 1; }
    const int col = x / BRICK_WIDTH;
    if (col > WORLD_COLS || col < 0) { return 1; }
    //printf("BRICK: %dx%d\n", row, col);
    return world[row][col];
}

void graphics_load() {
    printf("DEBUG: graphics_load()\n");
    /*init images*/
    /* TODO: Move world data from code to data files. */
    foreground = NULL; //load_image_as_rgba("lvl/foreground0.png");
    wscore = load_image_as_rgba("lvl/score.png");
    number = load_image_as_rgba("lvl/number.png");
    blood1 = load_image_as_rgba("bloods/blood4.png");
    snow = load_image_as_rgba("bloods/snow.png");
    sweapon1_1 = load_image_as_rgba("weapons/star1.png");
    sweapon1_2 = load_image_as_rgba("weapons/star2.png");

    /*inits some world images*/
    ninja = load_image_as_rgba("chars/ninja.new2.png");

    worldfloor = load_image_as_rgba("lvl/floor.png");
    worldbrick[0] = load_image_as_rgba("lvl/brick0.png");
    worldbrick[1] = load_image_as_rgba("lvl/brick1.png");
    worldbrick[2] = load_image_as_rgba("lvl/brick2.png");
    worldbrick[3] = load_image_as_rgba("lvl/brick3.png");
    dmgbrick[0] = load_image_as_rgba("lvl/dbrick0.png"); // tile 6: left facing spikes
    dmgbrick[1] = load_image_as_rgba("lvl/dbrick1.png"); // tile 7: up facing lava
    dmgbrick[2] = load_image_as_rgba("lvl/dbrick2.png"); // tile 8: right facing spikes
}


void graphics_free() {
    int i;

    printf("DEBUG: graphics_free()\n");

    free_surface(&foreground);
    free_surface(&wscore);
    free_surface(&number);
    free_surface(&blood1);
    free_surface(&snow);
    free_surface(&sweapon1_1);
    free_surface(&sweapon1_2);
    free_surface(&ninja);
    free_surface(&worldfloor);

    for(i = 0; i < BRICKS_WORLD; ++i) free_surface(&worldbrick[i]);
    for(i = 0; i < BRICKS_DAMAGE; ++i) free_surface(&dmgbrick[i]);
}


void load_current_world_from_file() {
    /***********************************************
     *creats the world(s)
     *reads in file that will constuct the world
     *file consits of:
    number of enemies
     *flollewed by enemy cooridinates x & y
     *leght of world
     *then world layout using the below world format
     ***************************************************/
    char lvlname[50];
    FILE* world_file;

    printf("DEBUG: load_current_world_from_file()\n");
    /* free the memory used by world, if any*/
    free_surface(&background);
    for(int i = 0; i < NMY; ++i) {
        enemies[i].is_visible = 0;
        for(int x = 0; x < NMY_FRAMES; ++x) { free_surface(&enemies[i].anim_frames[x]); free_surface(&enemies[i].anim_frames_flipped[x]); }
        for(int x = 0; x < NMY_DEATHS; ++x) { free_surface(&enemies[i].death_frames[x]); free_surface(&enemies[i].death_frames_flipped[x]); }
    }
    clear_particles();

    /********************************************
     *world stuff needed to make world
     *0 nothing
     *1floor
     *2brick
     *********************************************/

    /*initialises game world to all zeros*/
    for (int i = 0; i < WORLD_COLS; i++) {
        for (int t = 0; t < WORLD_ROWS; t++) {
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
    for (int i = 0; i < NMY; i++) {
        enemies[i].is_alive = 0;
    }

    /*world file to be used*/

    sprintf(lvlname, "lvl_files/lvl%d.txt", worldnum);
    world_file = fopen(lvlname, "r");

    if (world_file == NULL) {/*no more worlds to be read in*/
        show_victory_screen();
        printf("winner was reached\n");
        graphics_free();
        exit(EXIT_SUCCESS);
    }

    int x;
    fscanf(world_file, "%d", &x);

    enemymax = x; /*set max amount of enemies*/
    for (int i = 0; i < NMY && i < enemymax; i++) {/*set enemy destinations from file*/
        fscanf(world_file, "%d", &x);
        enemies[i].type = x;
        fscanf(world_file, "%d", &x);
        enemies[i].dest.x = x*BRICK_WIDTH;
        fscanf(world_file, "%d", &x);
        enemies[i].dest.y = x*BRICK_HEIGHT;
        enemies[i].is_alive = 1;
        enemies[i].jump_is_active = 0;
        enemies[i].is_flipped = 0;
        enemies[i].anim_frame = 0; /*what animation frame to start on*/
        enemies[i].anim_delay = 0; /*used to count in a delay funt*/
        enemies[i].is_visible = 0;

        /* TODO: Move enemy information out of code and into data files. */
        if (enemies[i].type == 0) {/*enemy type 0s quality*/
            enemies[i].anim_frames[0] = load_image_as_rgba("chars/turco0.png");
            enemies[i].anim_frames[1] = load_image_as_rgba("chars/turco1.png");

            /*death image, sword,*/
            enemies[i].death_frames[BYSWORD] = load_image_as_rgba("chars/turco2.png");
            enemies[i].death_frames[BYSTAR] = load_image_as_rgba("chars/turco3.png");
            enemies[i].death_frames[BYLAVA] = load_image_as_rgba("chars/turco4.png");

            enemies[i].jump_strength = -40;
            enemies[i].speed = 4;
        } else if (enemies[i].type == 1) {
            enemies[i].anim_frames[0] = load_image_as_rgba("chars/one0.png");
            enemies[i].anim_frames[1] = load_image_as_rgba("chars/one1.png");
            enemies[i].anim_frames[2] = load_image_as_rgba("chars/one2.png");
            enemies[i].anim_frames[3] = load_image_as_rgba("chars/one3.png");

            enemies[i].jump_strength = -45;
            enemies[i].speed = 5;
        } else if (enemies[i].type == 2) {
            enemies[i].anim_frames[0] = load_image_as_rgba("chars/rninja0.png");
            enemies[i].anim_frames[1] = load_image_as_rgba("chars/rninja1.png");
            enemies[i].anim_frames[2] = load_image_as_rgba("chars/rninja2.png");
            enemies[i].anim_frames[3] = load_image_as_rgba("chars/rninja3.png");
            enemies[i].anim_frames[4] = load_image_as_rgba("chars/rninja4.png");
            enemies[i].anim_frames[5] = load_image_as_rgba("chars/rninja5.png");

            enemies[i].jump_strength = -55;
            enemies[i].speed = 8;
        }

        for (int f = 0; f < NMY_FRAMES; ++f) { enemies[i].anim_frames_flipped[f] = mirror_surface(enemies[i].anim_frames[f]); }
        for (int f = 0; f < NMY_DEATHS; ++f) { enemies[i].death_frames_flipped[f] = mirror_surface(enemies[i].death_frames[f]); }
    }

    fscanf(world_file, "%d", &x);
    world_length = x;

    // START: Generate background image
    //sprintf(lvlname, "lvl/background%d.png", worldnum);
    //background = load_image_as_rgba(lvlname);
    background = create_rgba_surface(x * BRICK_WIDTH, SCREENHEIGHT);
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
    // END: Generate background image

    //sprintf(lvlname, "lvl/foreground%d.png", worldnum);
    //foreground = NULL; //load_image_as_rgba(lvlname);


    int i = 0;
    int t = 5;
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
    printf("DEBUG: set_screen()\n");
    const int world_height = 6;

    SDL_BlitSurface(background, &wrldps, screen, NULL);

    /* TODO: Move tile information out of code and into data files. */
    for (int i = 0; i < world_length; i++) {
        for (int t = 0; t < world_height; t++) {
            worlddest.y = t*BRICK_HEIGHT;
            worlddest.x = i*BRICK_WIDTH;
            if (world[t][i] == 1) {
                SDL_BlitSurface(worldfloor, NULL, background, &worlddest);
                SDL_BlitSurface(worldfloor, NULL, screen, &worlddest);
            } else if (world[t][i] == 2) {
                SDL_BlitSurface(worldbrick[0], NULL, background, &worlddest);
                SDL_BlitSurface(worldbrick[0], NULL, screen, &worlddest);
            } else if (world[t][i] == 3) {
                SDL_BlitSurface(worldbrick[1], NULL, background, &worlddest);
                SDL_BlitSurface(worldbrick[1], NULL, screen, &worlddest);
            } else if (world[t][i] == 4) {
                SDL_BlitSurface(worldbrick[2], NULL, background, &worlddest);
                SDL_BlitSurface(worldbrick[2], NULL, screen, &worlddest);
            } else if (world[t][i] == 5) {
                SDL_BlitSurface(worldbrick[3], NULL, background, &worlddest);
                SDL_BlitSurface(worldbrick[3], NULL, screen, &worlddest);
            } else if (world[t][i] == 6) {
                SDL_BlitSurface(dmgbrick[0], NULL, background, &worlddest);
                SDL_BlitSurface(dmgbrick[0], NULL, screen, &worlddest);
            } else if (world[t][i] == 7) {
                SDL_BlitSurface(dmgbrick[1], NULL, background, &worlddest);
                SDL_BlitSurface(dmgbrick[1], NULL, screen, &worlddest);
            } else if (world[t][i] == 8) {
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
            load_current_world_from_file(); // Doesn't take worldnum as a parameter
        }
    } else if (dest.x > SCREENWIDTH / 2) {/*moves screen if past half screen lenght*/
        dest.x = SCREENWIDTH / 2;
        wrldps.x += MOVERL;
        for (i = 0; i < enemymax; i++) { enemies[i].dest.x -= MOVERL; }
    } else if (dest.x < 0) {/*can't run off the left side*/
        dest.x = 0;
    }
}


void spawn_snow_particles() {
    int i = 0;
    do {
        const int x = rand() % (SCREENWIDTH * 2) + wrldps.x;
        const int y = 0;
        const float weight = 0.2 + (rand() % 10 * 0.01);
        spawn_particle(snow, x, y, -1 * bounded_rand(1, 5), bounded_rand(1, 5), weight, bounded_rand(3, 5));
    } while (++i < worldnum); // snow gets thicker the higher the worldnum
}


void spawn_blood_particles(const SDL_Rect target) {
    const int x = wrldps.x + target.x + (target.w / 2);
    const int y = target.y + (target.h / 2);

    spawn_particle(blood1, x, y, bounded_rand(1, 5), -1 * bounded_rand(3, 10), 1.0, bounded_rand(4, 5));
    spawn_particle(blood1, x, y, bounded_rand(0, 3), -1 * bounded_rand(6, 20), 1.0, bounded_rand(4, 6));
    spawn_particle(blood1, x, y, bounded_rand(1, 4) *-1, -1 * bounded_rand(2, 8), 1.0, bounded_rand(4, 7));
    spawn_particle(blood1, x, y, bounded_rand(1, 5), -1 * bounded_rand(3, 10), 1.0, bounded_rand(4, 8));
    spawn_particle(blood1, x, y, bounded_rand(0, 3), -1 * bounded_rand(4, 12), 1.0, bounded_rand(4, 9));
}
