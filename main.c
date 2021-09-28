#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "world.h"
#include "nmy.h"
#include "plyr.h"
#include "particles.h"
#include "image.h"

#define DTIME 5
#define MSECS_PER_FRAME 1000/30

void startScreen();
void playIntroMovie();
void letItSnow();
void updateScreen();
int delayMs(const unsigned int);

int main(int argc, char **argv) {
    /*********************************************************
     *Main
     *this runs the main event loop. Sets up video as well
     *as inits images.
     *********************************************************/

    Uint8* keys;
    Uint32 ticks, delay, tmp_ps;

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

    startScreen();

    /*init variables*/
    score = 100; /*start w/ 100 points since stars take poaints*/
    deaths = 0;
    attacklen = 0;
    jump = 0;
    gravity_compound = 0;
    tmp_ps = 0;
    worldnum = 0; /*set world number initially*/
    buildw(); /*builds the world*/

    graphics_load();

    set_screen();
    SDL_BlitSurface(ninja, &ninja_src, screen, &dest);
    SDL_Flip(screen);
    int skipLevel = 0;

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
                    dest.x -= MOVERL;
                    if (ninja_src.x == 60) {
                        ninja_src.x = 0;
                    } else {
                        ninja_src.x = 60;
                    }
                }
                if (keys[SDLK_RIGHT]) {
                    dest.x += MOVERL;
                    if (ninja_src.x == 240) {
                        ninja_src.x = 300;
                    } else {
                        ninja_src.x = 240;
                    }
                }
                if (keys[SDLK_UP]) {
                    if (jump == 0) {
                        jump = 1;
                        gravity_compound = JUMPMAX;
                    }
                }
                if (keys[SDLK_SPACE]) {
                    ninja_src.y = 160;
                    if (attack == 0) {
                        attack = ATTLEN;
                    }
                }
                break;
            case SDL_KEYUP:
                if (event.key.keysym.sym==SDLK_TAB && skipLevel == 1) {
                    skipLevel = 2;
                    break;
                }
                if (ninja_src.x >= 180) {
                    ninja_src.x = 180;
                } else {
                    ninja_src.x = 120;
                }

                if (keys[SDLK_LEFT]) {
                    dest.x -= MOVERL;
                    if (tmp_ps == 0) {
                        tmp_ps = 1;
                        ninja_src.x = 0;
                    } else {
                        tmp_ps = 0;
                        ninja_src.x = 60;
                    }
                }
                if (keys[SDLK_RIGHT]) {
                    dest.x += MOVERL;
                    if (tmp_ps == 0) {
                        tmp_ps = 1;
                        ninja_src.x = 300;
                    } else {
                        tmp_ps = 0;
                        ninja_src.x = 240;
                    }
                }

                break;
        }
        if (skipLevel == 2) {
            printf("DEBUG: Cheater, skipping level %d\n", worldnum);
            worldnum++;
            buildw();
            skipLevel = 0;
        }
        if (jump != 0) {
            ninja_src.y = 0;
            if (ninja_src.x >= 180) {
                ninja_src.x = 180;
            } else {
                ninja_src.x = 120;
            }
        }
        physics();

        //attacking
        if (attack != 0) {
            if (jump != 0) {
                if (ninja_src.x >= 180) {
                    ninja_src.x += 60;
                } else {
                    ninja_src.x -= 60;
                }
            } else {
                ninja_src.y = 160;
            }
            attack -= 1;
            killenemy();
        } //end of attacking

        ticks = SDL_GetTicks();

        SDL_BlitSurface(background, &wrldps, screen, NULL);
        SDL_BlitSurface(ninja, &ninja_src, screen, &dest);
        special_throw();
        enemyai();
        rprint(score);
        if (worldnum == 0) {
            letItSnow(snow, -2 + worldnum);
            letItSnow(ash, -2 + worldnum);
        } else {
            letItSnow(snow, -3 + worldnum);
        }
        drawParticles(screen);
        SDL_BlitSurface(foreground, &wrldps, screen, NULL);
        SDL_Flip(screen);

        delay = SDL_GetTicks() - ticks;
        if (delay >= MSECS_PER_FRAME) {
            delay = 0;
        } else {
            delay = MSECS_PER_FRAME - delay;
        }
        SDL_Delay(delay);
        world_mover();
    }

    graphics_free();

    return 8008135;
}


void letItSnow(SDL_Surface *precipitation, int snowFrequency) {
    static int snowLimiter = 0;
    if (snowFrequency < 0) {
        if (snowLimiter++ < 0) { return; }
        snowLimiter = snowFrequency;
    }
    int i = 0;
    do {
        int x = rand() % (640 * 2) + wrldps.x;
        int y = 0;
        float weight = 0.2 + (rand() % 10 * 0.01);
        addParticle(precipitation, x, y, (rand() % 4 + 1) * -1, rand() % 4 + 1, weight, (rand() % 2 + 4));
    } while (++i < snowFrequency);
}

void startScreen() {
    SDL_Surface *start_screen_surface;
    unsigned int msecs_waited;
    const unsigned int max_wait_length = 3000;

    printf("DEBUG: startScreen\n");

    start_screen_surface = loadImageAsSurface("lvl/start_screen.png");

    SDL_BlitSurface(start_screen_surface, NULL, screen, NULL);
    SDL_PumpEvents();
    updateScreen();


    /* wait 6000 milliseconds for the user to press spacebar; if they
     * don't, run the intro movie. */
    while (TRUE) {
        msecs_waited = 0;
        while(msecs_waited < max_wait_length) {
            msecs_waited += 10;
            if (delayMs(10)) {
                SDL_FreeSurface(start_screen_surface);
                return;
            }
        }
        playIntroMovie();
        SDL_BlitSurface(start_screen_surface, NULL, screen, NULL);
        updateScreen();
    }
}

void playIntroMovie() {

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

    printf("DEBUG: playIntroMovie()\n");

    color = SDL_MapRGB(screen->format, 0, 0, 0);

    lightn = loadImageAsSurface("intro_mov/lightening.png");
    SDL_BlitSurface(lightn, NULL, screen, NULL);
    updateScreen();
    SDL_FreeSurface(lightn);
    if (delayMs(60 * 5)) return;

    backdrop = loadImageAsSurface("intro_mov/backdrop.png");
    SDL_BlitSurface(backdrop, NULL, screen, NULL);
    updateScreen();
    if (delayMs(60 * 5)) return;

    ndest.y = 200;
    ndest.x = 480;
    bardest.x = 0;
    bardest.y = 0;
    bardest.h = 240;
    bardest.w = 640;

    rl = loadImageAsSurface("intro_mov/redleg.png");
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
        SDL_Flip(screen);
        if (delayMs(30)) return;
    }
    SDL_FreeSurface(rl);

    bl = loadImageAsSurface("intro_mov/blackleg.png");
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
        SDL_Flip(screen);
        if (delayMs(30)) return;
    }
    SDL_FreeSurface(bl);

    grass = loadImageAsSurface("intro_mov/grass.png");
    SDL_BlitSurface(grass, NULL, screen, NULL);
    SDL_Flip(screen);
    SDL_FreeSurface(grass);
    if (delayMs(60 * 2)) return;

    black = loadImageAsSurface("intro_mov/black.png");
    red = loadImageAsSurface("intro_mov/red.png");
    ndest.x = 0;
    ndest.y = SCREENHEIGHT / 2;
    SDL_BlitSurface(black, NULL, screen, &ndest);
    ndest.x = SCREENWIDTH / 2;
    ndest.y = 0;
    SDL_BlitSurface(red, NULL, screen, &ndest);
    SDL_Flip(screen);
    SDL_FreeSurface(black);
    SDL_FreeSurface(red);
    if (delayMs(60 * 5)) return;

    /*****************************************
     *beutiful text
     ******************************************/

    ndest.x = 20;
    ndest.y = 50;

    SDL_FillRect(screen, NULL, color);
    SDL_BlitSurface(backdrop, NULL, screen, NULL);
    txt1 = loadImageAsSurface("intro_mov/txt1.png");
    SDL_BlitSurface(txt1, NULL, screen, &ndest);
    SDL_Flip(screen);
    SDL_FreeSurface(txt1);
    if (delayMs(textDelayMs)) return;
    ndest.x += 50;
    ndest.y += 50;
    txt2 = loadImageAsSurface("intro_mov/txt2.png");
    SDL_BlitSurface(txt2, NULL, screen, &ndest);
    SDL_Flip(screen);
    SDL_FreeSurface(txt2);
    if (delayMs(textDelayMs)) return;
    ndest.x += 50;
    ndest.y += 50;
    txt3 = loadImageAsSurface("intro_mov/txt3.png");
    SDL_BlitSurface(txt3, NULL, screen, &ndest);
    SDL_Flip(screen);
    SDL_FreeSurface(txt3);
    if (delayMs(textDelayMs)) return;
    ndest.x += 50;
    ndest.y += 50;
    txt4 = loadImageAsSurface("intro_mov/txt4.png");
    SDL_BlitSurface(txt4, NULL, screen, &ndest);
    SDL_Flip(screen);
    SDL_FreeSurface(txt4);
    if (delayMs(textDelayMs)) return;
    ndest.x += 50;
    ndest.y += 50;
    txt5 = loadImageAsSurface("intro_mov/txt5.png");
    SDL_BlitSurface(txt5, NULL, screen, &ndest);
    SDL_Flip(screen);
    SDL_FreeSurface(txt5);
    if (delayMs(textDelayMs)) return;
    ndest.x += 50;
    ndest.y += 50;
    txt6 = loadImageAsSurface("intro_mov/txt6.png");
    SDL_BlitSurface(txt6, NULL, screen, &ndest);
    SDL_Flip(screen);
    SDL_FreeSurface(txt6);
    if (delayMs(textDelayMs)) return;
    /*end text*/

    ninjaborn = loadImageAsSurface("intro_mov/ninja_born.png");
    SDL_FillRect(screen, NULL, color);
    SDL_BlitSurface(backdrop, NULL, screen, NULL);
    SDL_BlitSurface(ninjaborn, NULL, screen, NULL);
    SDL_Flip(screen);
    SDL_FreeSurface(backdrop);
    SDL_FreeSurface(ninjaborn);
    if (delayMs(60 * 30)) return;

    /* free the memory we've used for the movie */
}

int delayMs(const unsigned int msToDelay) {
    int msecs_waited = 0;
    while(msecs_waited < msToDelay) {
        msecs_waited += 10;
        SDL_Delay(10);
        if (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    exit(EXIT_SUCCESS);
                    break; /* unreachable, but here for clarity */
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_SPACE) {
                        return 1;
                    }
                    continue;
                default:
                    continue;
            }
        }
    }
    return 0;
}

void updateScreen() {
    SDL_Flip(screen);
}

