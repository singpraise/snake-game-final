#ifndef QUEUE_H
#define QUEUE_H

#include "types.h"

/*
 * 鏈結式 Queue（FIFO）
 *
 * 在貪食蛇中的角色：
 *   - body  ：rear = 蛇頭（enqueue 新頭）, front = 蛇尾（dequeue 舊尾）
 *   - input ：儲存玩家連續按下的方向，每幀依序處理
 */

typedef struct QueueNode {
    Point data;
    struct QueueNode *next;
} QueueNode;

typedef struct {
    QueueNode *front; /* 佇列前端（蛇尾 / 最舊輸入） */
    QueueNode *rear;  /* 佇列後端（蛇頭 / 最新輸入） */
    int size;
} Queue;

typedef struct DirNode {
    Direction dir;
    struct DirNode *next;
} DirNode;

typedef struct {
    DirNode *front;
    DirNode *rear;
    int size;
} DirQueue;

Queue    *queue_create(void);
void      queue_destroy(Queue *q);
int       queue_enqueue(Queue *q, Point p);
int       queue_dequeue(Queue *q, Point *out);
int       queue_peek_rear(const Queue *q, Point *out);
int       queue_peek_front(const Queue *q, Point *out);
int       queue_size(const Queue *q);
Queue    *queue_clone(const Queue *q);

DirQueue *dir_queue_create(void);
void      dir_queue_destroy(DirQueue *q);
int       dir_queue_enqueue(DirQueue *q, Direction dir);
int       dir_queue_dequeue(DirQueue *q, Direction *out);
int       dir_queue_size(const DirQueue *q);

#endif /* QUEUE_H */
