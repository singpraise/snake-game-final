#include <stdlib.h>
#include "snake.h"
#include "memory.h"
#include "game_config.h"
#include "obstacles.h"
#include "score.h"
#include "audio.h"

#include <stdio.h>
#include <time.h>

static int is_opposite_direction(Direction a, Direction b)
{
    return (a == DIR_UP    && b == DIR_DOWN)  ||
           (a == DIR_DOWN  && b == DIR_UP)    ||
           (a == DIR_LEFT  && b == DIR_RIGHT) ||
           (a == DIR_RIGHT && b == DIR_LEFT);
}

static Point point_step(Point p, Direction dir)
{
    switch (dir) {
    case DIR_UP:    p.y++; break;
    case DIR_DOWN:  p.y--; break;
    case DIR_LEFT:  p.x--; break;
    case DIR_RIGHT: p.x++; break;
    }
    return p;
}

static void game_apply_input(GameState *game)
{
    if (game == NULL || game->input == NULL || game->paused) {
        return;
    }

    Direction pending;

    while (dir_queue_dequeue(game->input, &pending) == 0) {
        if (!is_opposite_direction(pending, game->dir)) {
            game->next_dir = pending;
        }
    }

    if (!is_opposite_direction(game->next_dir, game->dir)) {
        game->dir = game->next_dir;
    }
}

static int snake_hit_wall(const GameState *game, Point head)
{
    return head.x < 0 || head.x >= game->width ||
           head.y < 0 || head.y >= game->height;
}

static int snake_hit_self_board(const GameState *game, Point new_head, int will_grow)
{
    if (game == NULL || game->board == NULL || game->body == NULL) {
        return 1;
    }

    Cell cell = board_get(game->board, new_head.x, new_head.y);

    if (cell != CELL_SNAKE && cell != CELL_SNAKE_HEAD) {
        return 0;
    }

    if (!will_grow) {
        Point tail;
        if (queue_peek_front(game->body, &tail) == 0 &&
            tail.x == new_head.x && tail.y == new_head.y) {
            return 0;
        }
    }

    return 1;
}

static void board_apply_move(Board *board, Point old_head, Point old_tail,
                             Point new_head, int will_grow)
{
    board_set(board, old_head.x, old_head.y, CELL_SNAKE);
    board_set(board, new_head.x, new_head.y, CELL_SNAKE_HEAD);

    if (!will_grow) {
        board_set(board, old_tail.x, old_tail.y, CELL_EMPTY);
    }
}

static void game_level_up(GameState *game)
{
    int new_level = 1 + game->foods_eaten / FOODS_PER_LEVEL;

    if (new_level <= game->level) {
        return;
    }

    game->level = new_level;
    game->tick_ms = game_config_tick_for_level(game->mode, game->difficulty, game->level);
    obstacles_spawn_random(game, game_config_obstacles_per_level(game->difficulty));
}

static void game_end(GameState *game)
{
    game->game_over = 1;
    game_high_score_commit(game);
    audio_play(SFX_GAME_OVER);
}

const char *game_mode_name(GameMode mode)
{
    switch (mode) {
    case MODE_ENHANCED: return "Enhanced";
    case MODE_CUSTOM:   return "Custom";
    default:            return "Classic";
    }
}

const char *game_difficulty_name(Difficulty difficulty)
{
    switch (difficulty) {
    case DIFF_EASY:    return "Simple";
    case DIFF_NORMAL:  return "Medium";
    case DIFF_HARD:    return "Harder";
    case DIFF_EXTREME: return "Hardest";
    default:           return "Medium";
    }
}

const char *game_difficulty_name_zh(Difficulty difficulty)
{
    switch (difficulty) {
    case DIFF_EASY:    return "簡單";
    case DIFF_NORMAL:  return "中間";
    case DIFF_HARD:    return "更難";
    case DIFF_EXTREME: return "最難";
    default:           return "中間";
    }
}

void food_spawn(GameState *game)
{
    if (game == NULL || game->board == NULL || game->width <= 0 || game->height <= 0) {
        return;
    }

    int max_cells = game->width * game->height;
    int attempt = 0;

    do {
        game->food.x = rand() % game->width;
        game->food.y = rand() % game->height;

        if (board_get(game->board, game->food.x, game->food.y) == CELL_EMPTY) {
            Cell food_type = CELL_FOOD;
            if (game->mode == MODE_ENHANCED && (rand() % 100) < BONUS_FOOD_CHANCE) {
                food_type = CELL_BONUS_FOOD;
            }
            board_set(game->board, game->food.x, game->food.y, food_type);
            return;
        }

        attempt++;
    } while (attempt < max_cells);

    game->food.x = -1;
    game->food.y = -1;
}

int game_save_snapshot(GameState *game)
{
    if (game == NULL || game->body == NULL || game->board == NULL ||
        game->undo_stack == NULL) {
        return -1;
    }

    MoveSnapshot snap;
    snap.body = queue_clone(game->body);
    if (snap.body == NULL) {
        return -1;
    }
    snap.board = board_clone(game->board);
    if (snap.board == NULL) {
        queue_destroy(snap.body);
        return -1;
    }
    snap.food         = game->food;
    snap.score        = game->score;
    snap.dir          = game->dir;
    snap.level        = game->level;
    snap.foods_eaten  = game->foods_eaten;
    snap.tick_ms      = game->tick_ms;
    snap.mode         = game->mode;
    snap.difficulty   = game->difficulty;

    if (stack_push(game->undo_stack, snap) != 0) {
        snapshot_free(&snap);
        return -1;
    }
    return 0;
}

int game_undo(GameState *game)
{
    if (game == NULL || game->undo_stack == NULL) {
        return -1;
    }

    MoveSnapshot snap;
    if (stack_pop(game->undo_stack, &snap) != 0) {
        return -1;
    }

    queue_destroy(game->body);
    board_destroy(game->board);

    game->body        = snap.body;
    game->board       = snap.board;
    game->food        = snap.food;
    game->score       = snap.score;
    game->dir         = snap.dir;
    game->next_dir    = snap.dir;
    game->level       = snap.level;
    game->foods_eaten = snap.foods_eaten;
    game->tick_ms     = snap.tick_ms;
    game->mode        = snap.mode;
    game->difficulty  = snap.difficulty;
    game->length      = queue_size(game->body);
    game->game_over   = 0;
    game->paused      = 0;
    audio_play(SFX_UNDO);

    snap.body  = NULL;
    snap.board = NULL;

    return 0;
}

void snake_body_print(const GameState *game)
{
    if (game == NULL || game->body == NULL) {
        return;
    }

    Point head, tail;
    if (queue_peek_rear(game->body, &head) != 0 ||
        queue_peek_front(game->body, &tail) != 0) {
        printf("  (empty body)\n");
        return;
    }

    printf("  Head (queue rear): (%d, %d)\n", head.x, head.y);
    printf("  Tail (queue front): (%d, %d)\n", tail.x, tail.y);
    printf("  Body (tail->head): ");
    for (QueueNode *node = game->body->front; node != NULL; node = node->next) {
        printf("(%d,%d) ", node->data.x, node->data.y);
    }
    printf("\n");
}

void game_queue_direction(GameState *game, Direction dir)
{
    if (game == NULL || game->input == NULL || game->game_over || game->paused) {
        return;
    }
    dir_queue_enqueue(game->input, dir);
}

void game_toggle_pause(GameState *game)
{
    if (game != NULL && !game->game_over) {
        game->paused = !game->paused;
        audio_play(SFX_PAUSE);
    }
}

int game_update(GameState *game)
{
    if (game == NULL || game->body == NULL || game->board == NULL) {
        return -1;
    }
    if (game->game_over) {
        return 1;
    }
    if (game->paused) {
        return 2;
    }

    game_apply_input(game);

    Point old_head;
    Point old_tail;
    if (queue_peek_rear(game->body, &old_head) != 0 ||
        queue_peek_front(game->body, &old_tail) != 0) {
        return -1;
    }
    Point new_head = point_step(old_head, game->dir);

    if (snake_hit_wall(game, new_head)) {
        game_end(game);
        return 1;
    }

    Cell target = board_get(game->board, new_head.x, new_head.y);

    if (target == CELL_OBSTACLE) {
        game_end(game);
        return 1;
    }

    int will_grow = (target == CELL_FOOD || target == CELL_BONUS_FOOD);

    if (snake_hit_self_board(game, new_head, will_grow)) {
        game_end(game);
        return 1;
    }

    if (game_save_snapshot(game) != 0) {
        return -1;
    }

    if (queue_enqueue(game->body, new_head) != 0) {
        MoveSnapshot rollback;
        if (stack_pop(game->undo_stack, &rollback) == 0) {
            snapshot_free(&rollback);
        }
        return -1;
    }

    if (will_grow) {
        int points = (target == CELL_BONUS_FOOD) ? SCORE_BONUS_FOOD : SCORE_NORMAL_FOOD;
        int old_level = game->level;

        game->score += points;
        game->length++;
        game->foods_eaten++;
        audio_play(target == CELL_BONUS_FOOD ? SFX_BONUS : SFX_EAT);

        if (game->mode == MODE_ENHANCED) {
            game_level_up(game);
            if (game->level > old_level) {
                audio_play(SFX_LEVEL_UP);
            }
        }
    } else {
        queue_dequeue(game->body, NULL);
    }

    board_apply_move(game->board, old_head, old_tail, new_head, will_grow);

    if (will_grow) {
        food_spawn(game);
    }

    return 0;
}

void game_render(const GameState *game)
{
    if (game == NULL || game->board == NULL) {
        return;
    }

    for (int y = game->height - 1; y >= 0; y--) {
        for (int x = 0; x < game->width; x++) {
            Cell cell = board_get(game->board, x, y);
            putchar(board_cell_to_char(cell));
            putchar(' '); /* 每格加空白，讓盤面寬度貼近 HUD 文字、向右對齊滿版 */
        }
        putchar('\n');
    }
}
