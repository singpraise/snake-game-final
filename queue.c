#include "queue.h"
#include "memory.h"

/*
 * malloc / free 配對：
 *   queue_create      ↔ queue_destroy (SAFE_FREE Queue 本體)
 *   queue_enqueue     ↔ queue_dequeue (SAFE_FREE QueueNode)
 *   dir_queue_create  ↔ dir_queue_destroy
 *   dir_queue_enqueue ↔ dir_queue_dequeue (SAFE_FREE DirNode)
 */

/*
 * queue_create ── 初始化空 Queue
 *
 * Queue *q：回傳指向 heap 上 Queue 結構的指標
 * front / rear：皆為 NULL，表示尚無節點
 * 釋放：queue_destroy() → dequeue 所有節點 + SAFE_FREE(q)
 */
Queue *queue_create(void)
{
    Queue *q = (Queue *)mem_malloc(sizeof(Queue));
    if (q == NULL) {
        return NULL;
    }

    q->front = NULL; /* 指標：佇列前端（蛇尾），初始無節點 */
    q->rear  = NULL; /* 指標：佇列後端（蛇頭），初始無節點 */
    q->size  = 0;
    return q;
}

void queue_destroy(Queue *q)
{
    if (q == NULL) {
        return;
    }
    Point discarded;
    while (queue_dequeue(q, &discarded) == 0) {
    }
    q->front = NULL;
    q->rear  = NULL;
    q->size  = 0;
    SAFE_FREE(q);
}

int queue_enqueue(Queue *q, Point p)
{
    if (q == NULL) {
        return -1;
    }

    QueueNode *node = (QueueNode *)mem_malloc(sizeof(QueueNode));
    if (node == NULL) {
        return -1;
    }
    node->data = p;
    node->next = NULL;

    if (q->rear == NULL) {
        q->front = node;
        q->rear  = node;
    } else {
        q->rear->next = node;
        q->rear = node;
    }
    q->size++;
    return 0;
}

int queue_dequeue(Queue *q, Point *out)
{
    if (q == NULL || q->front == NULL) {
        return -1;
    }

    QueueNode *node = q->front;
    if (out != NULL) {
        *out = node->data;
    }

    q->front = node->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    SAFE_FREE(node);
    q->size--;
    return 0;
}

int queue_peek_rear(const Queue *q, Point *out)
{
    if (q == NULL || q->rear == NULL || out == NULL) {
        return -1;
    }
    *out = q->rear->data;
    return 0;
}

int queue_peek_front(const Queue *q, Point *out)
{
    if (q == NULL || q->front == NULL || out == NULL) {
        return -1;
    }
    *out = q->front->data;
    return 0;
}

int queue_size(const Queue *q)
{
    return q == NULL ? 0 : q->size;
}

Queue *queue_clone(const Queue *q)
{
    if (q == NULL) {
        return NULL;
    }

    Queue *copy = queue_create();
    if (copy == NULL) {
        return NULL;
    }

    for (QueueNode *node = q->front; node != NULL; node = node->next) {
        if (queue_enqueue(copy, node->data) != 0) {
            queue_destroy(copy);
            return NULL;
        }
    }
    return copy;
}

/*
 * dir_queue_create ── 初始化方向輸入佇列
 * 結構與 Queue 相同：front / rear 指標 + malloc 本體
 */
DirQueue *dir_queue_create(void)
{
    DirQueue *q = (DirQueue *)mem_malloc(sizeof(DirQueue));
    if (q == NULL) {
        return NULL;
    }
    q->front = NULL;
    q->rear  = NULL;
    q->size  = 0;
    return q;
}

void dir_queue_destroy(DirQueue *q)
{
    if (q == NULL) {
        return;
    }
    Direction discarded;
    while (dir_queue_dequeue(q, &discarded) == 0) {
    }
    q->front = NULL;
    q->rear  = NULL;
    q->size  = 0;
    SAFE_FREE(q);
}

int dir_queue_enqueue(DirQueue *q, Direction dir)
{
    if (q == NULL) {
        return -1;
    }

    DirNode *node = (DirNode *)mem_malloc(sizeof(DirNode));
    if (node == NULL) {
        return -1;
    }
    node->dir  = dir;
    node->next = NULL;

    if (q->rear == NULL) {
        q->front = node;
        q->rear  = node;
    } else {
        q->rear->next = node;
        q->rear = node;
    }
    q->size++;
    return 0;
}

int dir_queue_dequeue(DirQueue *q, Direction *out)
{
    if (q == NULL || q->front == NULL) {
        return -1;
    }

    DirNode *node = q->front;
    if (out != NULL) {
        *out = node->dir;
    }

    q->front = node->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    SAFE_FREE(node);
    q->size--;
    return 0;
}

int dir_queue_size(const DirQueue *q)
{
    return q == NULL ? 0 : q->size;
}
