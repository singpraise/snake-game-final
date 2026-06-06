#include "init.h"
#include "memory.h"
#include "game_config.h"
#include "obstacles.h"
#include "score.h"

#include <stdlib.h>
#include <time.h>

/* ── 內部輔助：計算蛇身各節座標（index=0 為蛇頭） ── */
static Point segment_pos(int start_x, int start_y, int index, Direction dir)
{
    Point p = { start_x, start_y };

    switch (dir) {
    case DIR_RIGHT: p.x = start_x - index; break;
    case DIR_LEFT:  p.x = start_x + index; break;
    case DIR_UP:    p.y = start_y + index; break;
    case DIR_DOWN:  p.y = start_y - index; break;
    default:        break;
    }
    return p;
}

static int point_in_bounds(int x, int y, int width, int height)
{
    return x >= 0 && x < width && y >= 0 && y < height;
}

/*
 * snake_body_fits_on_board ── 任意方向初始蛇身邊界驗證
 *
 * 逐節計算 segment_pos，確認全部落在 [0, width) × [0, height)。
 *
 * 各方向身體延伸規則（蛇頭在 start，身體在 index 增大方向）：
 *   DIR_RIGHT → 身體向左（x 遞減）  需 start_x - (length-1) >= 0
 *   DIR_LEFT  → 身體向右（x 遞增）  需 start_x + (length-1) < width
 *   DIR_UP    → 身體向下（y 遞增）  需 start_y + (length-1) < height
 *   DIR_DOWN  → 身體向上（y 遞減）  需 start_y - (length-1) >= 0
 */
int snake_body_fits_on_board(int width, int height,
                             int start_x, int start_y,
                             int length, Direction dir)
{
    if (width <= 0 || height <= 0 || length <= 0) {
        return 0;
    }

    for (int i = 0; i < length; i++) {
        Point p = segment_pos(start_x, start_y, i, dir);
        if (!point_in_bounds(p.x, p.y, width, height)) {
            return 0;
        }
    }
    return 1;
}

/*
 * snake_body_create ── 初始化蛇身 Queue
 *
 * 先通過 snake_body_fits_on_board 驗證，再配置 heap。
 * 配置：1 個 Queue + length 個 QueueNode（各 mem_malloc）。
 */
Queue *snake_body_create(int width, int height,
                         int start_x, int start_y,
                         int length, Direction dir)
{
    if (!snake_body_fits_on_board(width, height, start_x, start_y, length, dir)) {
        return NULL;
    }

    Queue *body = queue_create();
    if (body == NULL) {
        return NULL;
    }

    for (int i = length - 1; i >= 0; i--) {
        Point p = segment_pos(start_x, start_y, i, dir);
        if (queue_enqueue(body, p) != 0) {
            queue_destroy(body);
            return NULL;
        }
    }

    return body;
}

static GameState *game_create_impl(int width, int height,
                                 int initial_length, Direction initial_dir,
                                 GameMode mode, Difficulty difficulty)
{
    if (width < 3 || height < 3 || initial_length < 1) {
        return NULL;
    }
    if (initial_length >= width * height) {
        return NULL;
    }

    int start_x = width / 2;
    int start_y = height / 2;

    /* 任意方向：建立前先驗證所有節點座標 */
    if (!snake_body_fits_on_board(width, height,
                                  start_x, start_y,
                                  initial_length, initial_dir)) {
        return NULL;
    }

    GameState *game = (GameState *)mem_calloc(1, sizeof(GameState));
    if (game == NULL) {
        return NULL;
    }

    game->width      = width;
    game->height     = height;
    game->length     = initial_length;
    game->dir        = initial_dir;
    game->next_dir   = initial_dir;
    game->mode       = mode;
    game->difficulty = difficulty;
    game->level      = 1;
    game->foods_eaten = 0;
    game->paused     = 0;
    game->high_score = 0;
    game->tick_ms    = game_config_initial_tick_ms(mode, difficulty);

    game->body = snake_body_create(width, height,
                                   start_x, start_y,
                                   initial_length, initial_dir);
    if (game->body == NULL) {
        goto fail;
    }

    game->board = board_create(width, height);
    if (game->board == NULL) {
        goto fail;
    }

    game->input = dir_queue_create();
    if (game->input == NULL) {
        goto fail;
    }

    game->undo_stack = stack_create();
    if (game->undo_stack == NULL) {
        goto fail;
    }

    srand((unsigned int)time(NULL));

    for (QueueNode *node = game->body->front; node != NULL; node = node->next) {
        Point p = node->data;
        Cell cell = (node == game->body->rear) ? CELL_SNAKE_HEAD : CELL_SNAKE;
        board_set(game->board, p.x, p.y, cell);
    }

    game_high_score_load(game);

    if (mode == MODE_ENHANCED) {
        obstacles_spawn_random(game, game_config_initial_obstacles(difficulty));
    }

    food_spawn(game);
    return game;

fail:
    game_destroy(game);
    return NULL;
}

GameState *game_create(int width, int height, int initial_length)
{
    return game_create_impl(width, height, initial_length, DIR_RIGHT,
                            MODE_CLASSIC, DIFF_NORMAL);
}

GameState *game_create_classic(int width, int height, int initial_length,
                               Difficulty difficulty)
{
    return game_create_impl(width, height, initial_length, DIR_RIGHT,
                            MODE_CLASSIC, difficulty);
}

GameState *game_create_with_dir(int width, int height, int initial_length,
                                Direction initial_dir)
{
    return game_create_impl(width, height, initial_length, initial_dir,
                            MODE_CLASSIC, DIFF_NORMAL);
}

GameState *game_create_enhanced(int width, int height, int initial_length,
                                Difficulty difficulty)
{
    return game_create_impl(width, height, initial_length, DIR_RIGHT,
                            MODE_ENHANCED, difficulty);
}

void game_destroy(GameState *game)
{
    if (game == NULL) {
        return;
    }

    SAFE_DELETE(game->body, queue_destroy);
    SAFE_DELETE(game->board, board_destroy);
    SAFE_DELETE(game->input, dir_queue_destroy);
    SAFE_DELETE(game->undo_stack, stack_destroy);
    SAFE_FREE(game);
}
