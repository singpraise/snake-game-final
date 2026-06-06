#ifndef PLATFORM_H
#define PLATFORM_H

#include "snake.h"
#include "audio.h"

typedef enum {
    INPUT_NONE = 0,
    INPUT_QUIT = 1,
    INPUT_RESTART = 2,
    INPUT_PAUSE = 3
} InputAction;

void        platform_console_init(void);
void        platform_clear_screen(void);
void        platform_sleep_ms(int ms);
InputAction input_poll(GameState *game);

int         platform_high_score_load(void);
void        platform_high_score_save(int score);

void        platform_sound_init(void);
void        platform_sound_play(SfxId id);
void        platform_sound_set_enabled(int enabled);

#endif /* PLATFORM_H */
