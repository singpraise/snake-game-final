#ifndef TYPES_H
#define TYPES_H

typedef struct {
    int x;
    int y;
} Point;

typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

typedef enum {
    MODE_CLASSIC = 0,
    MODE_ENHANCED = 1,
    MODE_CUSTOM = 2
} GameMode;

typedef enum {
    DIFF_EASY = 0,    /* 簡單   - 最慢 */
    DIFF_NORMAL,      /* 中間 */
    DIFF_HARD,        /* 更難 */
    DIFF_EXTREME      /* 最難   - 最快 */
} Difficulty;

#endif /* TYPES_H */
