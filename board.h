#ifndef BOARD_H
#define BOARD_H

#include "types.h"
#include "queue.h"

/*
 * 2D Array 地圖
 *
 * cells[y][x] 讓碰撞與繪製的「單格查詢」皆為 O(1)。
 * 整張地圖掃描繪製仍為 O(寬×高)，但每格不再遍歷蛇身 Queue。
 */

typedef enum {
    CELL_EMPTY,
    CELL_SNAKE,
    CELL_SNAKE_HEAD,
    CELL_FOOD,
    CELL_BONUS_FOOD,  /* 強化版：高分食物 $ */
    CELL_OBSTACLE     /* 強化版：障礙物 # */
} Cell;

typedef struct {
    int width;
    int height;
    Cell **cells; /* cells[y][x]，以 malloc 配置二維陣列 */
} Board;

Board *board_create(int width, int height);
void   board_destroy(Board *board);
Board *board_clone(const Board *board);

Cell   board_get(const Board *board, int x, int y);
void   board_set(Board *board, int x, int y, Cell cell);
void   board_clear_all(Board *board);

/* 依 Queue 蛇身與食物座標，完整重建地圖（初始化 / undo 後使用） */
void board_rebuild(Board *board, const Queue *body, Point food);

char board_cell_to_char(Cell cell);

#endif /* BOARD_H */
