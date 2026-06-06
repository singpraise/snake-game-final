#include "game_ui.h"
#include "snake.h"
#include <stdio.h>

void game_ui_print_hud(const GameState *game)
{
    printf("=== C Snake Game [%s] ===\n", game_mode_name(game->mode));

    if (game->mode == MODE_ENHANCED || game->mode == MODE_CLASSIC) {
        printf("Speed: %s (%s) | Level: %d | High: %d\n",
               game_difficulty_name_zh(game->difficulty),
               game_difficulty_name(game->difficulty),
               game->level, game->high_score);
    } else {
        printf("High Score: %d | Speed: %dms\n", game->high_score, game->tick_ms);
    }

    printf("Score: %d | Length: %d | Speed: %dms | Dir: ",
           game->score, game->length, game->tick_ms);

    switch (game->dir) {
    case DIR_UP:    printf("UP");    break;
    case DIR_DOWN:  printf("DOWN");  break;
    case DIR_LEFT:  printf("LEFT");  break;
    case DIR_RIGHT: printf("RIGHT"); break;
    }

    if (game->food.x >= 0 && game->food.y >= 0) {
        printf(" | Food: (%d,%d)", game->food.x, game->food.y);
    } else {
        printf(" | Food: (none)");
    }

    if (game->paused) {
        printf("\n*** PAUSED ***");
    }

    printf("\n");
    printf("WASD/Arrows move | U Undo | P Pause | M Mute | R Restart | Q Quit\n");
    if (game->mode == MODE_ENHANCED) {
        printf("Legend: @ head  o body  * food(+10)  $(+30)  # obstacle\n\n");
    } else {
        printf("\n");
    }
}

void game_ui_print_game_over(const GameState *game)
{
    printf("\n========================================\n");
    if (game->game_over) {
        printf("  Game Over! Final score: %d\n", game->score);
        if (game->score >= game->high_score && game->score > 0) {
            printf("  *** New High Score! ***\n");
        }
    } else {
        printf("  Thanks for playing! Score: %d\n", game->score);
    }
    printf("========================================\n");
}
