#include "board.h"
#include "memory.h"

#include <string.h>

/*
 * malloc / free 配對：
 *   mem_malloc(Board)       ↔ board_destroy (SAFE_FREE)
 *   mem_malloc(Cell* 列陣列) ↔ board_destroy
 *   mem_malloc(Cell 每列)    ↔ board_destroy
 */

/*
 * board_create ── 初始化 2D 地圖（二重指標）
 *
 * 記憶體配置（共 1 + height + height×width 語意上為 height+1 次 malloc）：
 *   ① mem_malloc(Board)          → board 指標
 *   ② mem_malloc(Cell * × height) → board->cells（列指標陣列）
 *   ③ 每列 mem_malloc(Cell × width) → board->cells[y]（該列格子）
 *
 * 存取：board->cells[y][x]
 *   cells     是 Cell **（指向列指標陣列）
 *   cells[y]  是 Cell * （指向第 y 列）
 *   cells[y][x] 是 Cell   （格子值）
 *
 * 釋放：board_destroy() 逐列 SAFE_FREE 再 SAFE_FREE board
 */
Board *board_create(int width, int height)
{
    if (width <= 0 || height <= 0) {
        return NULL;
    }

    /* ① 配置 Board 結構本體 */
    Board *board = (Board *)mem_malloc(sizeof(Board));
    if (board == NULL) {
        return NULL;
    }

    board->width  = width;
    board->height = height;

    /* ② 配置「列指標陣列」：height 個 Cell* */
    board->cells = (Cell **)mem_malloc((size_t)height * sizeof(Cell *));
    if (board->cells == NULL) {
        SAFE_FREE(board);
        return NULL;
    }

    /* ③ 為每一列配置 width 個 Cell */
    for (int y = 0; y < height; y++) {
        board->cells[y] = (Cell *)mem_malloc((size_t)width * sizeof(Cell));
        if (board->cells[y] == NULL) {
            /* 配置失敗：free 已成功的列與 board，遵守黃金法則 */
            for (int i = 0; i < y; i++) {
                SAFE_FREE(board->cells[i]);
            }
            SAFE_FREE(board->cells);
            SAFE_FREE(board);
            return NULL;
        }
        memset(board->cells[y], 0, (size_t)width * sizeof(Cell));
    }

    return board;
}

void board_destroy(Board *board)
{
    if (board == NULL) {
        return;
    }

    if (board->cells != NULL) {
        for (int y = 0; y < board->height; y++) {
            SAFE_FREE(board->cells[y]);
        }
        SAFE_FREE(board->cells);
    }

    SAFE_FREE(board);
}

Board *board_clone(const Board *board)
{
    if (board == NULL) {
        return NULL;
    }

    Board *copy = board_create(board->width, board->height);
    if (copy == NULL) {
        return NULL;
    }

    for (int y = 0; y < board->height; y++) {
        memcpy(copy->cells[y], board->cells[y],
               (size_t)board->width * sizeof(Cell));
    }

    return copy;
}

Cell board_get(const Board *board, int x, int y)
{
    if (board == NULL || board->cells == NULL) {
        return CELL_EMPTY;
    }
    if (x < 0 || x >= board->width || y < 0 || y >= board->height) {
        return CELL_EMPTY;
    }
    return board->cells[y][x];
}

void board_set(Board *board, int x, int y, Cell cell)
{
    if (board == NULL || board->cells == NULL) {
        return;
    }
    if (x < 0 || x >= board->width || y < 0 || y >= board->height) {
        return;
    }
    board->cells[y][x] = cell;
}

void board_clear_all(Board *board)
{
    if (board == NULL || board->cells == NULL) {
        return;
    }

    for (int y = 0; y < board->height; y++) {
        memset(board->cells[y], 0, (size_t)board->width * sizeof(Cell));
    }
}

void board_rebuild(Board *board, const Queue *body, Point food)
{
    if (board == NULL || body == NULL) {
        return;
    }

    board_clear_all(board);

    for (QueueNode *node = body->front; node != NULL; node = node->next) {
        Cell cell = (node == body->rear) ? CELL_SNAKE_HEAD : CELL_SNAKE;
        board_set(board, node->data.x, node->data.y, cell);
    }

    if (board_get(board, food.x, food.y) == CELL_EMPTY) {
        board_set(board, food.x, food.y, CELL_FOOD);
    }
}

char board_cell_to_char(Cell cell)
{
    switch (cell) {
    case CELL_EMPTY:      return '.';
    case CELL_SNAKE:      return 'o';
    case CELL_SNAKE_HEAD: return '@';
    case CELL_FOOD:       return '*';
    case CELL_BONUS_FOOD: return '$';
    case CELL_OBSTACLE:   return '#';
    }
    return '?';
}
