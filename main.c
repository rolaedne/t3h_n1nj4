#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "world.h"
#include "nmy.h"
#include "plyr.h"
#include "particles.h"
#include "image.h"
#include "utils.h"
#include "screens.h"
#include "controls.h"
#include "special.h"

#define DTIME 5
#define MSECS_PER_FRAME 1000/30

void show_start_screen();
void play_intro_movie();
void draw_score_ui(SDL_Surface *screen);
void update_screen();
void draw_bounding_boxes(SDL_Surface *screen);

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *screen_texture;


int main() {
    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    atexit(SDL_Quit);

    window = SDL_CreateWindow(
        "t3h N1nj4 redux",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        1024, 768,
        SDL_WINDOW_RESIZABLE
    );
    if (window == NULL) {
        fprintf(stderr, "Error creating game window: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    //SDL_ShowCursor(0);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        fprintf(stderr, "Error creating game renderer: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
    SDL_RenderSetLogicalSize(renderer, SCREENWIDTH, SCREENHEIGHT);

    screen_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, SCREENWIDTH, SCREENHEIGHT);
    screen = create_rgba_surface(SCREENWIDTH, SCREENHEIGHT);

    init_controls();

    show_start_screen();

    /*init variables*/
    player.score = 100; /*start w/ 100 points since stars take poaints*/
    player.deaths = 0;
    player.is_jumping = FALSE;
    player.is_running = FALSE;
    player.is_looking_up = FALSE;
    player.is_looking_down = FALSE;
    player.gravity_compound = 0;
    worldnum = 0; /*set world number initially*/
    graphics_load();
    init_special_attack();
    load_current_world_from_file(); /*builds the world*/
    center_viewport_on_target(player.x, player.y);
    //printf("Viewport: x: %d, y: %d, w: %d, h: %d\n", vp.x, vp.y, vp.w, vp.h);

    Uint32 ticks, delay, tmp_ps = 0;
    Boolean pause_enemies = FALSE;
    Boolean show_bounding_boxes = FALSE;

    printf("DEBUG: entering main loop\n");
    while (TRUE) {
        Inputs inputs = check_inputs();

        if (inputs.left ^ inputs.right) { // XOR, don't move left and right at the same time
            player.is_facing_right = inputs.right;
            player.is_running = TRUE;
            player.x += player.is_facing_right ? MOVERL : -MOVERL; // TODO: Actual player movement should be in player_physics?
        }
        if (inputs.up ^ inputs.down) { // XOR, don't look up and down at the same time
            player.is_looking_up = inputs.up;
            player.is_looking_down = inputs.down;
        } else {
            player.is_looking_up = FALSE;
            player.is_looking_down = FALSE;
        }

        if (inputs.mouse_special_attack || inputs.special_attack) {
            const int source_x = player.x + player.w / 2;
            const int source_y = player.y;
            special_attack(inputs.mouse_button, source_x - vp.x, source_y - vp.y, inputs.mouse_x, inputs.mouse_y); // TODO: use world x,y for special attacks, not screen x,y
        }

        if (inputs.jump) {
            if (!player.is_jumping) {
                player.is_jumping = TRUE;
                player.gravity_compound = JUMPMAX;
            }
        }
        if (inputs.mouse_attack || inputs.attack) {
            if (player.attack == 0) {
                player.attack = ATTLEN;
            }
        }
        // TODO: Move debug input logic and debug flags/functionality into a debug.c/.h
        if (inputs.debug_skip_level) {
            printf("DEBUG: Cheater, skipping level %d\n", worldnum);
            worldnum++;
            load_current_world_from_file();
        }

        if (inputs.debug_pause_enemies) {
            pause_enemies = !pause_enemies;
        }

        if (inputs.debug_show_bounding_boxes) {
            show_bounding_boxes = !show_bounding_boxes;
        }

        ticks = SDL_GetTicks();
        player_physics();
        special_attack_tick();
        if (!pause_enemies) {
            enemy_ai(); // enemies_tick
        }
        spawn_snow_particles(); // precipitation_tick

        // Check if the player has reach the right edge of the world (aka, did they beat the current level)
        if (player.x > vp.max_x + vp.w - (player.h / 2)) {
            worldnum++;
            load_current_world_from_file();
        }

        center_viewport_on_target(player.x, player.y); // move viewport to track player x,y

        // Draw everything in the right order
        draw_world_background(screen);
        draw_player(screen);
        draw_enemies(screen);
        draw_special_attack(screen);
        draw_particles(screen);
        if (show_bounding_boxes) {
            draw_bounding_boxes(screen);
        }
        draw_score_ui(screen);

        // Update the actual display now that we've finished drawing
        update_screen();

        delay = SDL_GetTicks() - ticks;
        if (delay >= MSECS_PER_FRAME) {
            delay = 0;
        } else {
            delay = MSECS_PER_FRAME - delay;
        }
        SDL_Delay(delay);
        player.is_running = FALSE;
    }

    graphics_free();

    return 8008135;
}

void draw_bounding_box(const bbox box, SDL_Surface *screen, const Uint32 color) {
    SDL_Rect rects[4];
    const int view_x = box.x - vp.x;
    const int view_y = box.y - vp.y;
    // TOP
    rects[0].x = view_x;
    rects[0].y = view_y;
    rects[0].w = box.w;
    rects[0].h = 1;
    // BOTTOM
    rects[1].x = view_x;
    rects[1].y = view_y + box.h;
    rects[1].w = box.w;
    rects[1].h = 1;
    // LEFT
    rects[2].x = view_x;
    rects[2].y = view_y;
    rects[2].w = 1;
    rects[2].h = box.h;
    // RIGHT
    rects[3].x = view_x + box.w;
    rects[3].y = view_y;
    rects[3].w = 1;
    rects[3].h = box.h;

    SDL_FillRects(screen, rects, 4, color);
}

void draw_bounding_boxes(SDL_Surface *screen) {
    draw_bounding_box(get_player_box(&player),        screen, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF)); // player box is white
    draw_bounding_box(get_player_hit_box(&player),    screen, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0x00)); // player hit box is yellow
    draw_bounding_box(get_player_attack_box(&player), screen, SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00)); // player attack box is red

    // enemies
    const Uint32 box_color =    SDL_MapRGB(screen->format, 0x80, 0x80, 0x80); // enemy box is grey
    const Uint32 attack_color = SDL_MapRGB(screen->format, 0x80, 0x00, 0x00); // enemy attack box is red-ish
    for (int i = 0; i < NMY; ++i) {
        enemy *e = &enemies[i];
        draw_bounding_box(get_enemy_box(e),        screen, box_color);
        draw_bounding_box(get_enemy_attack_box(e), screen, attack_color);
    }

    // special attacks
    const Uint32 special_color = SDL_MapRGB(screen->format, 0x80, 0x00, 0x80); // special box is magenta-ish
    // TODO: Add this after we convert specials to world coordinates

    // tiles?
}

int get_length(int val) {
    int val_length = 0;
    do { val_length++; } while ((val /= 10) > 0);
    return val_length;
}

void draw_score_ui(SDL_Surface *screen) {
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
    ndest.x = SCREENHEIGHT;
    bardest.x = 0;
    bardest.y = 0;
    bardest.h = SCREENHEIGHT / 2;
    bardest.w = SCREENWIDTH;

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

    bbox bloodSpawn = get_player_box(&player);
    int vertical_accumulator = 0;

    int delayWait = 750; // Give the player a moment to see how they died
    while (!delay_ms_unskippable(MSECS_PER_FRAME) && delayWait > 0) {
        player_physics();
        enemy_ai();
        spawn_snow_particles();

        bloodSpawn.x = player.x;
        bloodSpawn.y = player.y;
        bloodSpawn.y -= vertical_accumulator++;
        spawn_blood_particles(bloodSpawn);
        const int old_x = bloodSpawn.x;
        bloodSpawn.x = old_x - 10 + bounded_rand(0, 20);
        spawn_blood_particles(bloodSpawn);
        bloodSpawn.x = old_x;

        center_viewport_on_target(player.x, player.y); // move viewport to track player x,y

        draw_world_background(screen);
        draw_player(screen);
        draw_enemies(screen);
        draw_particles(screen);
        draw_score_ui(screen);

        update_screen();

        delayWait -= MSECS_PER_FRAME;
    }

    SDL_Rect corpse_dest = { 395, 295, 0, 0 };
    SDL_Rect corpse_src = { 300, 0, 60, 80 };
    SDL_Surface *death_screen = load_image_as_rgba("lvl/dead.png");
    SDL_BlitSurface(death_screen, NULL, screen, NULL); /*print dead screen*/
    SDL_BlitSurface(player.ninja, &corpse_src, screen, &corpse_dest);
    draw_score_ui(screen);
    update_screen();
    free_surface(&death_screen);

    player.is_dead = FALSE;
    while (!delay_ms_skippable(100));
    load_current_world_from_file();
}

// Game over (as winner) screen
void show_victory_screen() {
    printf("DEBUG: ya won, bro\n");
    SDL_Surface *winnerimg = load_image_as_rgba("lvl/winner.png");
    SDL_BlitSurface(winnerimg, NULL, screen, NULL);
    draw_score_ui(screen);
    update_screen();
    free_surface(&winnerimg);

    while(!delay_ms_skippable(100));
}

void update_screen() {
    SDL_UpdateTexture(screen_texture, NULL, screen->pixels, screen->pitch);
    SDL_RenderClear(renderer); // clear
    SDL_RenderCopy(renderer, screen_texture, NULL, NULL); // render the whole screen_texture
    SDL_RenderPresent(renderer); // update the display hardware
}
