/*
 * main_web.c — 網頁版進入點（Emscripten → WebAssembly）
 */
#include <stdio.h>
#include <emscripten.h>

#include "snake.h"
#include "memory.h"
#include "platform.h"
#include "game_ui.h"
#include "audio.h"
#include "editor.h"
#include "level.h"

typedef enum {
    APP_GAME = 0,
    APP_EDITOR = 1
} AppMode;

static GameState *game = NULL;
static GameMode g_mode = MODE_ENHANCED;
static Difficulty g_difficulty = DIFF_NORMAL;
static AppMode g_app = APP_GAME;

static void restart_game(void)
{
    if (game != NULL) {
        game_destroy(game);
        game = NULL;
    }

    if (g_mode == MODE_ENHANCED) {
        game = game_create_enhanced(32, 16, 3, g_difficulty);
    } else {
        game = game_create_classic(32, 16, 3, g_difficulty);
    }
}

static void start_custom_level(CustomLevel *level)
{
    if (game != NULL) {
        game_destroy(game);
        game = NULL;
    }
    game = game_create_from_level(level);
}

static void game_frame(void)
{
    if (game == NULL) {
        return;
    }

    platform_clear_screen();
    game_ui_print_hud(game);

    if (!game->game_over) {
        game_render(game);
    } else {
        game_ui_print_game_over(game);
        printf("\nPress R to restart or Q to quit.\n");
    }

    InputAction action = input_poll(game);
    if (action == INPUT_QUIT) {
        emscripten_cancel_main_loop();
        game_destroy(game);
        game = NULL;
        return;
    }
    if (action == INPUT_RESTART) {
        restart_game();
        return;
    }
    if (action == INPUT_PAUSE) {
        game_toggle_pause(game);
        return;
    }

    if (!game->game_over && !game->paused) {
        int result = game_update(game);
        if (result == 1) {
            platform_sleep_ms(400);
            return;
        }
        if (result == -1) {
            fprintf(stderr, "Update failed.\n");
            emscripten_cancel_main_loop();
            game_destroy(game);
            game = NULL;
            return;
        }
    }

    platform_sleep_ms(game->tick_ms);
}

static void editor_app_frame(void)
{
    if (!editor_is_active()) {
        g_app = APP_GAME;
        restart_game();
        return;
    }

    editor_render();

    if (editor_play_requested()) {
        CustomLevel *level = editor_take_level();
        if (level != NULL) {
            start_custom_level(level);
            level_destroy(level);
        }
        g_app = APP_GAME;
    }
}

static void app_frame(void)
{
    if (g_app == APP_EDITOR) {
        editor_app_frame();
    } else {
        game_frame();
    }
}

EMSCRIPTEN_KEEPALIVE
void web_restart(void)
{
    g_app = APP_GAME;
    restart_game();
}

EMSCRIPTEN_KEEPALIVE
void web_set_mode(int mode, int difficulty)
{
    g_app = APP_GAME;
    g_mode = (mode == 0) ? MODE_CLASSIC : MODE_ENHANCED;

    switch (difficulty) {
    case 0:  g_difficulty = DIFF_EASY;    break;
    case 2:  g_difficulty = DIFF_HARD;    break;
    case 3:  g_difficulty = DIFF_EXTREME; break;
    default: g_difficulty = DIFF_NORMAL;  break;
    }
    restart_game();
}

EMSCRIPTEN_KEEPALIVE
void web_start_editor(void)
{
    if (game != NULL) {
        game_destroy(game);
        game = NULL;
    }
    editor_start();
    g_app = APP_EDITOR;
}

EMSCRIPTEN_KEEPALIVE
int web_get_score(void)
{
    return game != NULL ? game->score : 0;
}

EMSCRIPTEN_KEEPALIVE
int web_is_game_over(void)
{
    return game != NULL && game->game_over;
}

EMSCRIPTEN_KEEPALIVE
const char *web_get_mode_name(void)
{
    if (game == NULL) {
        return "unknown";
    }
    return game_mode_name(game->mode);
}

EMSCRIPTEN_KEEPALIVE
int web_load_level_text(const char *text)
{
    CustomLevel *lvl = level_load_string(text);

    if (lvl == NULL) {
        return -1;
    }

    g_app = APP_GAME;
    start_custom_level(lvl);
    level_destroy(lvl);
    return 0;
}

int main(void)
{
    platform_console_init();
    audio_init();
    restart_game();
    if (game == NULL && g_app == APP_GAME) {
        fprintf(stderr, "Failed to initialize game.\n");
        return 1;
    }

    emscripten_set_main_loop(app_frame, 0, 1);
    return 0;
}
