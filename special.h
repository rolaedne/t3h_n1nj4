#ifndef SPECIAL_H
#define SPECIAL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    STAR = 0, FIRE = 1
} Special_Type;

void init_special_attack();
void special_attack(const Special_Type type, const int source_x, const int source_y, const int mouse_x, const int mouse_y);
void special_attack_tick();
void clear_special_attack();
void draw_special_attack(SDL_Surface *screen);

#ifdef __cplusplus
}
#endif

#endif /* SPECIAL_H */