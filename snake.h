#ifndef SNAKE_H
#define SNAKE_H

#include "types.h"
#include "board.h"
#include "queue.h"
#include "stack.h"

/*
 * 遊戲主狀態
 *
 * body        → Queue  ：蛇身（FIFO，rear=頭、front=尾）
 * board       → Board  ：2D Array 地圖（O(1) 碰撞 / 繪製查詢）
 * input       → DirQueue：玩家方向輸入緩衝（FIFO）
 * undo_stack  → Stack  ：移動前快照（LIFO，供復原）
 */
typedef struct {
    int width;
    int height;

    Queue    *body;
    Board    *board;
    DirQueue *input;
    Stack    *undo_stack;

    int length;
    Direction dir;
    Direction next_dir;
    Point food;
    int score;
    int game_over;

    /* 強化版欄位 */
    GameMode   mode;
    Difficulty difficulty;
    int level;
    int foods_eaten;
    int paused;
    int high_score;
    int tick_ms;
} GameState;

GameState *game_create(int width, int height, int initial_length);
GameState *game_create_classic(int width, int height, int initial_length,
                               Difficulty difficulty);
GameState *game_create_with_dir(int width, int height, int initial_length,
                                Direction initial_dir);
GameState *game_create_enhanced(int width, int height, int initial_length,
                                Difficulty difficulty);

int snake_body_fits_on_board(int width, int height,
                             int start_x, int start_y,
                             int length, Direction dir);

Queue *snake_body_create(int width, int height,
                         int start_x, int start_y,
                         int length, Direction dir);

void       game_destroy(GameState *game);
void       food_spawn(GameState *game);

int game_save_snapshot(GameState *game);
int game_undo(GameState *game);

void snake_body_print(const GameState *game);
void game_queue_direction(GameState *game, Direction dir);
void game_toggle_pause(GameState *game);

/*
 * 一幀更新：處理輸入 → 碰撞檢測 → Queue 移動
 * 回傳 0=成功, 1=遊戲結束, 2=暫停中, -1=錯誤
 */
int game_update(GameState *game);

void game_render(const GameState *game);

const char *game_mode_name(GameMode mode);
const char *game_difficulty_name(Difficulty difficulty);
const char *game_difficulty_name_zh(Difficulty difficulty);

#endif /* SNAKE_H */
