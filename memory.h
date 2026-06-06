#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

/*
 * ═══════════════════════════════════════════════════════════
 *  黃金法則：有多少 malloc / calloc，就必須有多少 free
 * ═══════════════════════════════════════════════════════════
 *
 *  本專案所有 heap 配置統一經由 mem_malloc() / mem_calloc()，
 *  所有釋放統一經由 SAFE_FREE() / SAFE_DELETE()。
 *  禁止在其他檔案直接呼叫 malloc / calloc / free。
 *
 *  ┌─────────────────────────┬──────────────────────────────┐
 *  │ 配置 (mem_*)            │ 釋放 (SAFE_* / destroy)      │
 *  ├─────────────────────────┼──────────────────────────────┤
 *  │ mem_malloc Queue        │ queue_destroy → SAFE_FREE    │
 *  │ mem_malloc QueueNode    │ queue_dequeue → SAFE_FREE    │
 *  │ mem_malloc DirQueue     │ dir_queue_destroy            │
 *  │ mem_malloc DirNode      │ dir_queue_dequeue            │
 *  │ mem_malloc Board        │ board_destroy                │
 *  │ mem_malloc Cell* 列陣列 │ board_destroy                │
 *  │ mem_malloc Cell 每列    │ board_destroy                │
 *  │ mem_malloc Stack        │ stack_destroy                │
 *  │ mem_malloc StackNode    │ stack_pop / discard_oldest   │
 *  │ mem_calloc GameState    │ game_destroy                 │
 *  └─────────────────────────┴──────────────────────────────┘
 *
 *  編譯時加 -DMEM_DEBUG 可啟用配置計數，程式結束時應為 0。
 */

void  *mem_malloc(size_t size);
void  *mem_calloc(size_t count, size_t size);
void   mem_free(void *ptr);

int    mem_live_count(void);   /* MEM_DEBUG：目前未釋放區塊數 */
int    mem_assert_clean(void); /* 結束時檢查，非 0 表示 leak */

#define SAFE_FREE(ptr)        \
    do {                      \
        mem_free(ptr);        \
        (ptr) = NULL;         \
    } while (0)

#define SAFE_DELETE(ptr, fn)  \
    do {                      \
        if ((ptr) != NULL) {  \
            fn(ptr);          \
            (ptr) = NULL;     \
        }                     \
    } while (0)

#endif /* MEMORY_H */
