#include "memory.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef MEM_DEBUG
static int s_live_allocs = 0;
#endif

void *mem_malloc(size_t size)
{
    void *ptr = malloc(size);
#ifdef MEM_DEBUG
    if (ptr != NULL) {
        s_live_allocs++;
    }
#endif
    return ptr;
}

void *mem_calloc(size_t count, size_t size)
{
    void *ptr = calloc(count, size);
#ifdef MEM_DEBUG
    if (ptr != NULL) {
        s_live_allocs++;
    }
#endif
    return ptr;
}

void mem_free(void *ptr)
{
    if (ptr == NULL) {
        return;
    }

#ifdef MEM_DEBUG
    s_live_allocs--;
#endif
    free(ptr);
}

int mem_live_count(void)
{
#ifdef MEM_DEBUG
    return s_live_allocs;
#else
    return 0;
#endif
}

int mem_assert_clean(void)
{
#ifdef MEM_DEBUG
    if (s_live_allocs != 0) {
        fprintf(stderr,
                "[MEMORY] Leak: %d block(s) still allocated\n",
                s_live_allocs);
        return s_live_allocs;
    }
    fprintf(stderr, "[MEMORY] OK: all malloc/calloc paired with free\n");
#endif
    return 0;
}
