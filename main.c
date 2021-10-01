#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "world.h"
#include "nmy.h"
#include "plyr.h"
#include "particles.h"
#include "image.h"
#include "utils.h"
#include "screens.h"

#define DTIME 5
#define MSECS_PER_FRAME 1000/60

void show_start_screen();
void play_intro_movie();
void draw_score_ui();
void update_screen();

int main() {
    /*********************************************************
     *Main
     *this runs the main event loop. Sets up video as well
     *as inits images.
     *********************************************************/

    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    atexit(SDL_Quit);

    screen = SDL_SetVideoMode(640, 480, 0, SDL_DOUBLEBUF); /*creates screen*/
    if (screen == NULL) {
        fprintf(stderr, "Error setting video mode 640x480: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_WM_SetCaption("t3h N1nj4 redux", NULL); /*sets caption 4 prog*/

    show_start_screen();

    /*init variables*/
    player.score = 100; /*start w/ 100 points since stars take poaints*/
    player.deaths = 0;
    player.sattack_length = 0;
    player.is_jumping = FALSE;
    player.is_running = FALSE;
    player.gravity_compound = 0;
    worldnum = 0; /*set world number initially*/
    graphics_load();
    load_current_world_from_file(); /*builds the world*/

    //SDL_BlitSurface(ninja, &player.ninja_src, screen, &dest);
    //update_screen();

    int skipLevel = 0;
    Uint32 ticks, delay, tmp_ps = 0;
    Uint8* keys;
    while (SDL_PollEvent(&event) != -1) {//main even loop
        keys = SDL_GetKeyState(NULL);

        switch (event.type) {
            case SDL_QUIT: //quits program
                exit(0);
                break;
            case SDL_KEYDOWN: //registars key down
                if (event.key.keysym.sym==SDLK_TAB && skipLevel == 0) {
                    skipLevel = 1;
                    break;
                }
                if (keys[SDLK_DOWN]) {
                    special();
                }
                if (keys[SDLK_LEFT]) {
                    player.x -= MOVERL;
                    player.is_facing_right = FALSE;
                    player.is_running = TRUE;
                }
                if (keys[SDLK_RIGHT]) {
                    player.x += MOVERL;
                    player.is_facing_right = TRUE;
                    player.is_running = TRUE;
                }
                if (keys[SDLK_UP]) {
                    if (!player.is_jumping) {
                        player.is_jumping = TRUE;
                        player.gravity_compound = JUMPMAX;
                    }
                }
                if (keys[SDLK_SPACE]) {
                    if (player.attack == 0) {
                        player.attack = ATTLEN;
                    }
                }
                break;
            case SDL_KEYUP:
                if (event.key.keysym.sym==SDLK_TAB && skipLevel == 1) {
                    skipLevel = 2;
                    break;
                }
                if (keys[SDLK_LEFT]) {
                    player.x -= MOVERL;
                    player.is_facing_right = FALSE;
                    player.is_running = TRUE;
                }
                if (keys[SDLK_RIGHT]) {
                    player.x += MOVERL;
                    player.is_facing_right = TRUE;
                    player.is_running = TRUE;
                }
                break;
        }
        if (skipLevel == 2) {
            printf("DEBUG: Cheater, skipping level %d\n", worldnum);
            worldnum++;
            load_current_world_from_file();
            skipLevel = 0;
        }

        player_physics();

        if (player.attack > 0) {
            check_for_kills();
            player.attack--;
        }

        ticks = SDL_GetTicks();

        spawn_snow_particles(); // precipitation_tick
        SDL_BlitSurface(background, &wrldps, screen, NULL); // draw_background || draw_world
        draw_player();
        special_throw(); // special_attack_physics_tick && draw_special_attack
        enemy_ai(); // enemies_tick && draw_enemies
        draw_particles(screen);
        SDL_BlitSurface(foreground, &wrldps, screen, NULL); // draw_shadows || draw_overlays
        draw_score_ui();
        update_screen();

        delay = SDL_GetTicks() - ticks;
        if (delay >= MSECS_PER_FRAME) {
            delay = 0;
        } else {
            delay = MSECS_PER_FRAME - delay;
        }
        SDL_Delay(delay);
        world_mover();
        player.is_running = FALSE;
    }

    graphics_free();

    return 8008135;
}

int get_length(int val) {
    int val_length = 0;
    do { val_length++; } while ((val /= 10) > 0);
    return val_length;
}

void draw_score_ui() {
    SDL_Rect score_dest = { 10, 10 };
    SDL_BlitSurface(wscore, NULL, screen, &score_dest);

    int val = player.score;
    if (val <= 0) { val = 0; }

    SDL_Rect wdest = { score_dest.x + wscore->w + 10 + (10 * get_length(val)), score_dest.y };
    SDL_Rect didest = { 0, 0, 10, 13 };
    do {
        didest.x = (val % 10) * 11;
        SDL_BlitSurface(number, &didest, screen, &wdest);
        wdest.x -= 10;
    } while ((val /= 10) > 0);
}

// Initial player welcome screen. If they don't press spacebar, we'll show them an intro movie too.
void show_start_screen() {
    printf("DEBUG: startScreen\n");

    SDL_Surface *start_screen_surface = load_image_as_rgba("lvl/start_screen.png");

    SDL_BlitSurface(start_screen_surface, NULL, screen, NULL);
    SDL_PumpEvents();
    update_screen();

    while (TRUE) {
        if (delay_ms_skippable(3000)) {
            free_surface(&start_screen_surface);
            return;
        }
        play_intro_movie();
        SDL_BlitSurface(start_screen_surface, NULL, screen, NULL);
        update_screen();
    }
}

// The best intro movie
void play_intro_movie() {
    printf("DEBUG: playIntroMovie()\n");

    /***************************
     *the intro movei that gets
     *you into the plot
     ******************************/
    Uint8 color;
    int i;
    int textDelayMs = 60 * 15;
    SDL_Rect ndest, bardest;
    SDL_Surface *backdrop, *bl, *grass, *black, *lightn, *ninjaborn,
            *rl, *red, *txt1, *txt2, *txt3, *txt4, *txt5, *txt6;


    color = SDL_MapRGB(screen->format, 0, 0, 0);

    lightn = load_image_as_rgba("intro_mov/lightening.png");
    SDL_BlitSurface(lightn, NULL, screen, NULL);
    update_screen();
    SDL_FreeSurface(lightn);
    if (delay_ms_skippable(60 * 5)) return;

    backdrop = load_image_as_rgba("intro_mov/backdrop.png");
    SDL_BlitSurface(backdrop, NULL, screen, NULL);
    update_screen();
    if (delay_ms_skippable(60 * 5)) return;

    ndest.y = 200;
    ndest.x = 480;
    bardest.x = 0;
    bardest.y = 0;
    bardest.h = 240;
    bardest.w = 640;

    rl = load_image_as_rgba("intro_mov/redleg.png");
    for (i = 0; i < 50; i++) {
        SDL_BlitSurface(backdrop, NULL, screen, NULL);
        if ((i % 5) == 0) {
            ndest.x += 30;
        }

        SDL_BlitSurface(rl, NULL, screen, &ndest);

        if ((i % 5) == 0) {
            ndest.x -= 30;
        }
        SDL_BlitSurface(rl, NULL, screen, &ndest);
        SDL_FillRect(screen, &bardest, color);
        update_screen();
        if (delay_ms_skippable(30)) return;
    }
    SDL_FreeSurface(rl);

    bl = load_image_as_rgba("intro_mov/blackleg.png");
    ndest.x = 120;
    for (i = 0; i < 50; i++) {
        SDL_BlitSurface(backdrop, NULL, screen, NULL);
        if ((i % 5) == 0) {
            ndest.x -= 30;
        }

        SDL_BlitSurface(bl, NULL, screen, &ndest);

        if ((i % 5) == 0) {
            ndest.x += 30;
        }
        SDL_BlitSurface(bl, NULL, screen, &ndest);
        SDL_FillRect(screen, &bardest, color);
        update_screen();
        if (delay_ms_skippable(30)) return;
    }
    SDL_FreeSurface(bl);

    grass = load_image_as_rgba("intro_mov/grass.png");
    SDL_BlitSurface(grass, NULL, screen, NULL);
    update_screen();
    SDL_FreeSurface(grass);
    if (delay_ms_skippable(60 * 2)) return;

    black = load_image_as_rgba("intro_mov/black.png");
    red = load_image_as_rgba("intro_mov/red.png");
    ndest.x = 0;
    ndest.y = SCREENHEIGHT / 2;
    SDL_BlitSurface(black, NULL, screen, &ndest);
    ndest.x = SCREENWIDTH / 2;
    ndest.y = 0;
    SDL_BlitSurface(red, NULL, screen, &ndest);
    update_screen();
    SDL_FreeSurface(black);
    SDL_FreeSurface(red);
    if (delay_ms_skippable(60 * 5)) return;

    /*****************************************
     *beutiful text
     ******************************************/

    ndest.x = 20;
    ndest.y = 50;

    SDL_FillRect(screen, NULL, color);
    SDL_BlitSurface(backdrop, NULL, screen, NULL);
    txt1 = load_image_as_rgba("intro_mov/txt1.png");
    SDL_BlitSurface(txt1, NULL, screen, &ndest);
    update_screen();
    SDL_FreeSurface(txt1);
    if (delay_ms_skippable(textDelayMs)) return;
    ndest.x += 50;
    ndest.y += 50;
    txt2 = load_image_as_rgba("intro_mov/txt2.png");
    SDL_BlitSurface(txt2, NULL, screen, &ndest);
    update_screen();
    SDL_FreeSurface(txt2);
    if (delay_ms_skippable(textDelayMs)) return;
    ndest.x += 50;
    ndest.y += 50;
    txt3 = load_image_as_rgba("intro_mov/txt3.png");
    SDL_BlitSurface(txt3, NULL, screen, &ndest);
    update_screen();
    SDL_FreeSurface(txt3);
    if (delay_ms_skippable(textDelayMs)) return;
    ndest.x += 50;
    ndest.y += 50;
    txt4 = load_image_as_rgba("intro_mov/txt4.png");
    SDL_BlitSurface(txt4, NULL, screen, &ndest);
    update_screen();
    SDL_FreeSurface(txt4);
    if (delay_ms_skippable(textDelayMs)) return;
    ndest.x += 50;
    ndest.y += 50;
    txt5 = load_image_as_rgba("intro_mov/txt5.png");
    SDL_BlitSurface(txt5, NULL, screen, &ndest);
    update_screen();
    SDL_FreeSurface(txt5);
    if (delay_ms_skippable(textDelayMs)) return;
    ndest.x += 50;
    ndest.y += 50;
    txt6 = load_image_as_rgba("intro_mov/txt6.png");
    SDL_BlitSurface(txt6, NULL, screen, &ndest);
    update_screen();
    SDL_FreeSurface(txt6);
    if (delay_ms_skippable(textDelayMs)) return;
    /*end text*/

    ninjaborn = load_image_as_rgba("intro_mov/ninja_born.png");
    SDL_FillRect(screen, NULL, color);
    SDL_BlitSurface(backdrop, NULL, screen, NULL);
    SDL_BlitSurface(ninjaborn, NULL, screen, NULL);
    update_screen();
    SDL_FreeSurface(backdrop);
    SDL_FreeSurface(ninjaborn);
    if (delay_ms_skippable(60 * 30)) return;

    /* free the memory we've used for the movie */
}

// Death screen
void dead() {
    if (player.is_dead) { return; }
    printf("DEBUG: ya dead, boo.\n");
    player.deaths += 1;
    player.attack = 0;
    player.is_dead = TRUE;
    player.score -= 10; if (player.score < 0) player.score = 0;

    SDL_Rect bloodSpawn = { player.x, player.y, player.w, player.h };
    spawn_blood_particles(bloodSpawn);

    int vertical_accumulator = 0;

    int delayWait = 750; // Give the player a moment to see how they died
    while (!delay_ms_unskippable(MSECS_PER_FRAME) && delayWait > 0) {
        SDL_BlitSurface(background, &wrldps, screen, NULL);
        draw_player();
        bloodSpawn.x = player.x;
        bloodSpawn.y = player.y;
        const int old_x = bloodSpawn.x;
        bloodSpawn.y -= vertical_accumulator++;
        spawn_blood_particles(bloodSpawn);
        bloodSpawn.x = old_x - bounded_rand(10, 25);
        spawn_blood_particles(bloodSpawn);
        bloodSpawn.x = old_x + bounded_rand(10, 25);
        spawn_blood_particles(bloodSpawn);
        bloodSpawn.x = old_x;
        special_throw();
        player_physics();
        enemy_ai();
        draw_score_ui();
        spawn_snow_particles();
        draw_particles(screen);
        update_screen();

        delayWait -= MSECS_PER_FRAME;
    }

    SDL_Rect corpse_dest = { 395, 295, 0, 0 };
    SDL_Rect corpse_src = { 300, 0, 60, 80 };
    SDL_Surface *death_screen = load_image_as_rgba("lvl/dead.png");
    SDL_BlitSurface(death_screen, NULL, screen, NULL); /*print dead screen*/
    SDL_BlitSurface(player.ninja, &corpse_src, screen, &corpse_dest);
    draw_score_ui();
    update_screen();
    free_surface(&death_screen);

    player.sattack = 0;
    player.is_dead = FALSE;
    while (!delay_ms_skippable(100));
    load_current_world_from_file();
}

// Game over (as winner) screen
void show_victory_screen() {
    printf("DEBUG: ya won, bro\n");
    SDL_Surface *winnerimg = load_image_as_rgba("lvl/winner.png");
    SDL_BlitSurface(winnerimg, NULL, screen, NULL);
    draw_score_ui();
    update_screen();
    free_surface(&winnerimg);

    while(!delay_ms_skippable(100));
}

void update_screen() {
    SDL_Flip(screen);
}
