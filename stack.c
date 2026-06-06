#include "stack.h"
#include "memory.h"

/*
 * malloc / free 配對：
 *   mem_malloc(Stack)     ↔ stack_destroy (SAFE_FREE)
 *   mem_malloc(StackNode) ↔ stack_pop / stack_discard_oldest (SAFE_FREE)
 *
 * snapshot 內的 body / board 由 queue_destroy / board_destroy 釋放，
 * 不額外 malloc，但必須經 snapshot_free() 呼叫對應 destroy。
 */

void snapshot_free(MoveSnapshot *snap)
{
    if (snap == NULL) {
        return;
    }
    SAFE_DELETE(snap->body, queue_destroy);
    SAFE_DELETE(snap->board, board_destroy);
}

/*
 * stack_create ── 初始化空 Stack
 *
 * Stack *s：指向 heap 上的 Stack 結構
 * s->top：指向 StackNode 鏈結串列頂端，初始 NULL（空堆疊）
 * 釋放：stack_destroy() → pop 所有節點 + snapshot_free + SAFE_FREE(s)
 */
Stack *stack_create(void)
{
    Stack *s = (Stack *)mem_malloc(sizeof(Stack));
    if (s == NULL) {
        return NULL;
    }
    s->top  = NULL; /* 指標：堆疊頂，初始無節點 */
    s->size = 0;
    return s;
}

int stack_pop(Stack *s, MoveSnapshot *out)
{
    if (s == NULL || s->top == NULL) {
        return -1;
    }

    StackNode *node = s->top;
    MoveSnapshot snap = node->data;

    s->top = node->next;
    SAFE_FREE(node);
    s->size--;

    if (out != NULL) {
        *out = snap;
    } else {
        snapshot_free(&snap);
    }

    return 0;
}

static void stack_discard_oldest(Stack *s)
{
    if (s == NULL || s->top == NULL) {
        return;
    }

    if (s->top->next == NULL) {
        MoveSnapshot snap;
        if (stack_pop(s, &snap) == 0) {
            snapshot_free(&snap);
        }
        return;
    }

    StackNode *prev = NULL;
    StackNode *cur  = s->top;

    while (cur->next != NULL) {
        prev = cur;
        cur  = cur->next;
    }

    snapshot_free(&cur->data);
    prev->next = NULL;
    SAFE_FREE(cur);
    s->size--;
}

int stack_push(Stack *s, MoveSnapshot snap)
{
    if (s == NULL) {
        return -1;
    }

    StackNode *node = (StackNode *)mem_malloc(sizeof(StackNode));
    if (node == NULL) {
        /* malloc 失敗：snap 所有權仍屬呼叫端，須由呼叫端 snapshot_free */
        return -1;
    }

    node->data = snap;
    node->next = s->top;
    s->top = node;
    s->size++;

    while (s->size > UNDO_STACK_MAX) {
        stack_discard_oldest(s);
    }

    return 0;
}

void stack_destroy(Stack *s)
{
    if (s == NULL) {
        return;
    }

    MoveSnapshot snap;
    while (stack_pop(s, &snap) == 0) {
        snapshot_free(&snap);
    }
    SAFE_FREE(s);
}

int stack_peek(const Stack *s, MoveSnapshot *out)
{
    if (s == NULL || s->top == NULL || out == NULL) {
        return -1;
    }
    *out = s->top->data;
    return 0;
}

int stack_size(const Stack *s)
{
    return s == NULL ? 0 : s->size;
}
