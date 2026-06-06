#ifndef INIT_H
#define INIT_H

#include "snake.h"

/*
 * 初始化模組（init）
 *
 * 本模組負責以 pointer + malloc 建立遊戲所需的所有 heap 結構。
 * 黃金法則：每個 mem_malloc / mem_calloc 都有對應的 destroy / SAFE_FREE。
 *
 * 初始化呼叫順序：
 *   game_create() / game_create_with_dir() / game_create_enhanced()
 *     ├─ snake_body_fits_on_board()  → 任意方向邊界驗證
 *     ├─ snake_body_create()         → Queue + 多個 QueueNode (malloc)
 *     ├─ board_create()       → Board + Cell** + 各列 Cell* (malloc)
 *     ├─ dir_queue_create()   → DirQueue (malloc)
 *     └─ stack_create()       → Stack (malloc)
 */

#endif /* INIT_H */
