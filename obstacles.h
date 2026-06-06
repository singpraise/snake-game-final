#ifndef OBSTACLES_H
#define OBSTACLES_H

#include "snake.h"

/* 在空格上隨機放置障礙物（CELL_OBSTACLE）；回傳實際放置數量 */
int obstacles_spawn_random(GameState *game, int count);

#endif /* OBSTACLES_H */
