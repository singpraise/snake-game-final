#include "platform.h"
#include "audio.h"
#include "editor.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <stdio.h>

static int g_sound_enabled = 1;

#define KEY_QUEUE_CAP 64

static int key_queue[KEY_QUEUE_CAP];
static int key_head;
static int key_tail;

static void key_enqueue(int ch)
{
    int next = (key_tail + 1) % KEY_QUEUE_CAP;

    if (next == key_head) {
        return;
    }
    key_queue[key_tail] = ch;
    key_tail = next;
}

static int key_dequeue(int *out)
{
    if (key_head == key_tail) {
        return -1;
    }
    *out = key_queue[key_head];
    key_head = (key_head + 1) % KEY_QUEUE_CAP;
    return 0;
}

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
void web_send_key(int ch)
{
    if (editor_is_active()) {
        int result = editor_handle_key(ch);
        if (result == EDITOR_EXIT_QUIT) {
            editor_stop();
        }
        return;
    }
    key_enqueue(ch);
}

static void handle_direction_key(int ch, GameState *game)
{
    switch (ch) {
    case 'w': case 'W': case 38: game_queue_direction(game, DIR_UP);    break;
    case 's': case 'S': case 40: game_queue_direction(game, DIR_DOWN);  break;
    case 'a': case 'A': case 37: game_queue_direction(game, DIR_LEFT);  break;
    case 'd': case 'D': case 39: game_queue_direction(game, DIR_RIGHT); break;
    case 'u': case 'U': game_undo(game);                                break;
    default: break;
    }
}
#endif

void platform_console_init(void)
{
#ifdef __EMSCRIPTEN__
    EM_ASM({
        if (typeof window.snakeOnReady === 'function') {
            window.snakeOnReady();
        }
    });
#endif
}

void platform_clear_screen(void)
{
#ifdef __EMSCRIPTEN__
    EM_ASM({
        var el = document.getElementById('game-screen');
        if (el) {
            el.textContent = '';
        }
    });
#endif
}

void platform_sleep_ms(int ms)
{
#ifdef __EMSCRIPTEN__
    emscripten_sleep(ms);
#else
    (void)ms;
#endif
}

int platform_high_score_load(void)
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({
        var value = localStorage.getItem('snake_high_score');
        return value ? parseInt(value, 10) : 0;
    });
#else
    return 0;
#endif
}

void platform_high_score_save(int score)
{
#ifdef __EMSCRIPTEN__
    EM_ASM({
        localStorage.setItem('snake_high_score', String($0));
    }, score);
#else
    (void)score;
#endif
}

void platform_sound_init(void)
{
}

void platform_sound_set_enabled(int enabled)
{
    g_sound_enabled = enabled ? 1 : 0;
#ifdef __EMSCRIPTEN__
    EM_ASM({
        if (window.snakeAudio) {
            window.snakeAudio.enabled = $0 !== 0;
        }
    }, enabled);
#endif
}

void platform_sound_play(SfxId id)
{
#ifdef __EMSCRIPTEN__
    if (!g_sound_enabled) {
        return;
    }
    EM_ASM({
        if (window.snakeAudio && window.snakeAudio.play) {
            window.snakeAudio.play($0);
        }
    }, (int)id);
#else
    (void)id;
#endif
}

InputAction input_poll(GameState *game)
{
    int ch;

    if (game == NULL) {
        return INPUT_NONE;
    }

#ifndef __EMSCRIPTEN__
    (void)game;
    return INPUT_NONE;
#else
    while (key_dequeue(&ch) == 0) {
        if (ch == 'q' || ch == 'Q') {
            return INPUT_QUIT;
        }
        if (ch == 'r' || ch == 'R') {
            return INPUT_RESTART;
        }
        if (ch == 'p' || ch == 'P') {
            return INPUT_PAUSE;
        }
        if (ch == 'm' || ch == 'M') {
            audio_toggle();
            continue;
        }
        if (!game->game_over) {
            handle_direction_key(ch, game);
        }
    }
    return INPUT_NONE;
#endif
}
