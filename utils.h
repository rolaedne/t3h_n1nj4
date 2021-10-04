#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FALSE = 0, TRUE = 1
} Boolean;

int bounded_rand(int min, int max);

int delay_ms_unskippable(const unsigned int ms_to_delay);
int delay_ms_skippable(const unsigned int ms_to_delay);

typedef struct {
    int x;
    int y;
    int w;
    int h;
} bbox;

int bbox_collision(const bbox box1, const bbox box2);

#ifdef __cplusplus
}
#endif

#endif /* UTILS_H */