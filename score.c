#include "score.h"
#include "platform.h"

#include <stddef.h>

void game_high_score_load(GameState *game)
{
    if (game == NULL) {
        return;
    }
    game->high_score = platform_high_score_load();
}

void game_high_score_commit(GameState *game)
{
    if (game == NULL) {
        return;
    }

    if (game->score > game->high_score) {
        game->high_score = game->score;
        platform_high_score_save(game->high_score);
    }
}
