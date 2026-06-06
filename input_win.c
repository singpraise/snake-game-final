#include "platform.h"
#include "audio.h"

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#define HIGH_SCORE_FILE "snake_highscore.txt"

static int g_sound_enabled = 1;

void platform_console_init(void)
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

void platform_clear_screen(void)
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void platform_sleep_ms(int ms)
{
#ifdef _WIN32
    Sleep((DWORD)ms);
#else
    usleep((useconds_t)ms * 1000);
#endif
}

int platform_high_score_load(void)
{
    FILE *fp = fopen(HIGH_SCORE_FILE, "r");
    int score = 0;

    if (fp == NULL) {
        return 0;
    }
    if (fscanf(fp, "%d", &score) != 1) {
        score = 0;
    }
    fclose(fp);
    return score;
}

void platform_high_score_save(int score)
{
    FILE *fp = fopen(HIGH_SCORE_FILE, "w");

    if (fp == NULL) {
        return;
    }
    fprintf(fp, "%d\n", score);
    fclose(fp);
}

void platform_sound_init(void)
{
}

void platform_sound_set_enabled(int enabled)
{
    g_sound_enabled = enabled ? 1 : 0;
}

void platform_sound_play(SfxId id)
{
#ifdef _WIN32
    if (!g_sound_enabled) {
        return;
    }

    switch (id) {
    case SFX_EAT:
        Beep(880, 70);
        break;
    case SFX_BONUS:
        Beep(1175, 90);
        Beep(1400, 110);
        break;
    case SFX_LEVEL_UP:
        Beep(700, 60);
        Beep(900, 60);
        Beep(1100, 90);
        break;
    case SFX_GAME_OVER:
        Beep(220, 180);
        Beep(180, 260);
        break;
    case SFX_UNDO:
        Beep(520, 50);
        break;
    case SFX_PAUSE:
        Beep(420, 40);
        break;
    case SFX_EDITOR_PLACE:
        Beep(640, 35);
        break;
    case SFX_EDITOR_SAVE:
        Beep(760, 50);
        Beep(980, 70);
        break;
    case SFX_EDITOR_LOAD:
        Beep(980, 50);
        Beep(760, 70);
        break;
    default:
        break;
    }
#else
    (void)id;
#endif
}

#ifdef _WIN32
static void handle_direction_key(int ch, GameState *game)
{
    if (ch == 224 || ch == 0) {
        ch = _getch();
        switch (ch) {
        case 72: game_queue_direction(game, DIR_UP);    break;
        case 80: game_queue_direction(game, DIR_DOWN);  break;
        case 75: game_queue_direction(game, DIR_LEFT);  break;
        case 77: game_queue_direction(game, DIR_RIGHT); break;
        }
        return;
    }

    switch (ch) {
    case 'w': case 'W': game_queue_direction(game, DIR_UP);    break;
    case 's': case 'S': game_queue_direction(game, DIR_DOWN);  break;
    case 'a': case 'A': game_queue_direction(game, DIR_LEFT);  break;
    case 'd': case 'D': game_queue_direction(game, DIR_RIGHT); break;
    case 'u': case 'U': game_undo(game);                       break;
    default: break;
    }
}
#endif

InputAction input_poll(GameState *game)
{
    if (game == NULL) {
        return INPUT_NONE;
    }

#ifndef _WIN32
    (void)game;
    return INPUT_NONE;
#else
    if (!_kbhit()) {
        return INPUT_NONE;
    }

    int ch = _getch();
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
        return INPUT_NONE;
    }

    if (!game->game_over) {
        handle_direction_key(ch, game);
    }

    return INPUT_NONE;
#endif
}
