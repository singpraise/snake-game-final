/*
 * main.c — 程式進入點與遊戲主迴圈（Windows 原生版）
 */
#include <stdio.h>
#include "snake.h"
#include "memory.h"
#include "platform.h"
#include "game_ui.h"
#include "audio.h"
#include "editor.h"
#include "level.h"

#ifdef _WIN32
#include <conio.h>
#endif

static GameMode g_mode = MODE_CLASSIC;
static Difficulty g_difficulty = DIFF_NORMAL;

static int read_menu_choice(void)
{
#ifdef _WIN32
    return _getch();
#else
    return getchar();
#endif
}

static void print_main_menu(void)
{
    platform_clear_screen();
    printf("========================================\n");
    printf("       C Snake Game - Mode Select\n");
    printf("========================================\n\n");
    printf("  [1] Classic      - original snake\n");
    printf("  [2] Enhanced     - obstacles, levels, bonus\n");
    printf("  [3] Level Editor - design custom maps\n");
    printf("  [4] Quit\n\n");
    printf("Select mode (1-4): ");
}

static void choose_speed(void)
{
    int ch;

    printf("Speed / Difficulty:\n");
    printf("  [1] 簡單 (Simple)   - slowest\n");
    printf("  [2] 中間 (Medium)\n");
    printf("  [3] 更難 (Harder)\n");
    printf("  [4] 最難 (Hardest)  - fastest\n");
    printf("Select (1-4): ");
    ch = read_menu_choice();
    printf("%c\n", ch);

    if (ch == '1') {
        g_difficulty = DIFF_EASY;
    } else if (ch == '3') {
        g_difficulty = DIFF_HARD;
    } else if (ch == '4') {
        g_difficulty = DIFF_EXTREME;
    } else {
        g_difficulty = DIFF_NORMAL;
    }
}

static int choose_mode(void)
{
    int ch;

    print_main_menu();
    ch = read_menu_choice();
    printf("%c\n\n", ch);

    if (ch == '4' || ch == 'q' || ch == 'Q') {
        return 0;
    }
    if (ch == '3') {
        return 3;
    }
    if (ch == '2') {
        g_mode = MODE_ENHANCED;
        choose_speed();
        return 1;
    }

    g_mode = MODE_CLASSIC;
    choose_speed();
    return 1;
}

static GameState *create_game(void)
{
    if (g_mode == MODE_ENHANCED) {
        return game_create_enhanced(20, 15, 3, g_difficulty);
    }
    return game_create_classic(20, 15, 3, g_difficulty);
}

static void run_game_loop(GameState *game)
{
    while (1) {
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
            break;
        }
        if (action == INPUT_RESTART) {
            game_destroy(game);
            return;
        }
        if (action == INPUT_PAUSE) {
            game_toggle_pause(game);
            continue;
        }

        if (!game->game_over && !game->paused) {
            int result = game_update(game);
            if (result == 1) {
                platform_sleep_ms(400);
            } else if (result == -1) {
                fprintf(stderr, "Update failed.\n");
                break;
            }
        }

        platform_sleep_ms(game->tick_ms);
    }

    game_destroy(game);
}

static void run_custom_level(CustomLevel *level)
{
    GameState *game = game_create_from_level(level);

    if (game == NULL) {
        printf("Failed to start custom level.\n");
        return;
    }

    run_game_loop(game);
}

int main(void)
{
    int choice;

    platform_console_init();
    audio_init();

    choice = choose_mode();
    if (choice == 0) {
        return 0;
    }

    if (choice == 3) {
        EditorResult ed = editor_run_blocking();
        if (ed.code == EDITOR_EXIT_PLAY && ed.level != NULL) {
            run_custom_level(ed.level);
            level_destroy(ed.level);
        }
        return mem_assert_clean() != 0 ? 2 : 0;
    }

    while (1) {
        GameState *game = create_game();
        if (game == NULL) {
            fprintf(stderr, "Failed to initialize game.\n");
            return 1;
        }

        run_game_loop(game);

        platform_clear_screen();
        printf("Play again? [Y/N]: ");
        {
            int again = read_menu_choice();
            if (again != 'y' && again != 'Y') {
                break;
            }
        }
    }

    if (mem_assert_clean() != 0) {
        return 2;
    }

    return 0;
}
