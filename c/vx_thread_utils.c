/*
   vx_thread_utils.c — Tier 1: Threading primitives
*/
#include "vx_thread_utils.h"

vmath_thread_t vmath_thread_start(void* (*func)(void*), void* arg) {
    pthread_t thread;
    pthread_create(&thread, NULL, func, arg);
    return thread;
}

void vmath_thread_join(vmath_thread_t thread) {
    pthread_join(thread, NULL);
}

EXPORT void vx_sys_dump_ring_state(int win_id) {
    if (win_id < 0 || win_id >= MAX_WINDOWS) return;

    uint32_t mask   = L_R(g_ring.locked_mask);
    int      ready  = L_R(g_ring.ready_idx[win_id]);
    int      local  = L_R(g_ring.local_read[win_id]);
    int      wsi    = L_R(g_wsi_state[win_id]);
    int      offset = win_id * 4;
    uint32_t tenant_mask = (mask >> offset) & 0xF;

    printf("\n--- FREEZE AUTOPSY (Tenant %d) ---\n", win_id);
    printf("WSI State  : %d\n", wsi);
    printf("Ready Idx  : %d\n", ready);
    printf("Local Read : %d\n", local);
    printf("Slot Locks : [ %d | %d | %d | %d ]\n",
           (tenant_mask & 1) != 0, (tenant_mask & 2) != 0,
           (tenant_mask & 4) != 0, (tenant_mask & 8) != 0);
    printf("----------------------------------\n");
    fflush(stdout);
}
