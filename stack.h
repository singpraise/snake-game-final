#ifndef STACK_H
#define STACK_H

#include "board.h"
#include "queue.h"
#include "types.h"

/*
 * 鏈結式 Stack（LIFO）
 *
 * 在貪食蛇中的角色：
 *   undo_stack：每次移動前 push 一份快照，pop 可還原上一步
 *               （後進先出，符合「復原」語意）
 */

typedef struct {
    Queue     *body;
    Board     *board;
    Point      food;
    int        score;
    Direction  dir;
    int        level;
    int        foods_eaten;
    int        tick_ms;
    GameMode   mode;
    Difficulty difficulty;
} MoveSnapshot;

typedef struct StackNode {
    MoveSnapshot data;
    struct StackNode *next;
} StackNode;

#define UNDO_STACK_MAX 50 /* 限制 undo 深度，避免 Stack 無限增長佔用 heap */

typedef struct {
    StackNode *top; /* 指向堆疊頂端（最近一次的快照） */
    int size;
} Stack;

Stack *stack_create(void);
void   stack_destroy(Stack *s);
int    stack_push(Stack *s, MoveSnapshot snap); /* 超過 UNDO_STACK_MAX 時自動丟棄最舊快照 */
int    stack_pop(Stack *s, MoveSnapshot *out);
int    stack_peek(const Stack *s, MoveSnapshot *out);
int    stack_size(const Stack *s);
void   snapshot_free(MoveSnapshot *snap);

#endif /* STACK_H */
