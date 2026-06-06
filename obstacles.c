#include "obstacles.h"

#include <stdlib.h>

int obstacles_spawn_random(GameState *game, int count)
{
    int placed = 0;

    if (game == NULL || game->board == NULL || count <= 0) {
        return 0;
    }

    int max_cells = game->width * game->height;
    int attempts = 0;
    int max_attempts = max_cells * 4;

    while (placed < count && attempts < max_attempts) {
        int x = rand() % game->width;
        int y = rand() % game->height;

        if (board_get(game->board, x, y) == CELL_EMPTY) {
            board_set(game->board, x, y, CELL_OBSTACLE);
            placed++;
        }
        attempts++;
    }

    return placed;
}
