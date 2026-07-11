/*
   vx_thread_utils.h — Tier 1: Threading primitives
*/
#pragma once
#include "vx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ── Tiered Backoff Spinlock Wait ────────────────────────────────────────── */
static inline void vx_spin_wait(int* spin_count) {
    if (*spin_count < 1000) {
        _mm_pause();
    } else if (*spin_count < 2000) {
        #if defined(_WIN32)
            SwitchToThread();
        #else
            sched_yield();
        #endif
    } else {
        SLEEP_MS(1);
    }
    (*spin_count)++;
}

vmath_thread_t vmath_thread_start(void* (*func)(void*), void* arg);
void           vmath_thread_join(vmath_thread_t thread);

EXPORT void    vx_sys_dump_ring_state(int win_id);

#ifdef __cplusplus
}
#endif
