/*
   vx_memory_ring.c — Tier 1: Lock-free queues
*/
#include "vx_memory_ring.h"
#include "vx_thread_utils.h"

EXPORT RenderPacket* vx_stream_packet(int idx) {
    if (idx < 0 || idx >= RING_SIZE) {
        printf("[FATAL] -1 index requests");
        return NULL;
    }
    return &g_ring.packets[idx];
}

EXPORT int vx_stream_acquire(int win_id) {
    if (win_id < 0 || win_id >= MAX_WINDOWS) return -1;

    int ready  = L(g_ring.ready_idx[win_id]);
    int offset = win_id * 4;

    for (int i = 1; i <= 4; i++) {
        int local_curr = (ready == -1) ? -1 : (ready - offset);
        int next_local = (local_curr + i) % 4;
        int global_idx = offset + next_local;
        uint32_t bit   = (1u << global_idx);

        uint32_t expected = L(g_ring.locked_mask);

        while ((expected & bit) == 0) {
            if (CWX(g_ring.locked_mask, expected, expected | bit)) {
                return global_idx;
            }
        }
    }
    return -1;
}

EXPORT void vx_stream_commit(int win_id, int idx) {
    if (win_id < 0 || win_id >= MAX_WINDOWS) return;
    atomic_thread_fence(memory_order_release);
    S(g_ring.ready_idx[win_id], idx);
}

EXPORT void vx_stream_init(int win_id, VulkanDeviceContext* dev_ctx) {
    if (win_id < 0 || win_id >= MAX_WINDOWS) return;

    S(g_wsi_state[win_id], 0);
    int timeout = 2000;
    int spin_count = 0;

    while (L(g_render_busy[win_id]) || L(g_transfer_busy[win_id])) {
        if (spin_count >= 2000) { timeout--; }
        if (timeout <= 0) {
            printf("[C-FATAL] Threads failed to release busy flags "
                   "for Tenant %d. Aborting init to prevent corruption.\n", win_id);
            return;
        }
        vx_spin_wait(&spin_count);
    }

    g_device_ctx[win_id] = *dev_ctx;

    S(g_wsi_generation[win_id], 0);
    memset(&g_wsi_ctx[win_id][0], 0, sizeof(VulkanSwapchainContext));
    memset(&g_wsi_ctx[win_id][1], 0, sizeof(VulkanSwapchainContext));

    int      offset      = win_id * 4;
    uint32_t tenant_mask = 0xFu << offset;
    FA(g_ring.locked_mask, ~tenant_mask);

    S(g_ring.ready_idx[win_id],  -1);
    S(g_ring.local_read[win_id], -1);

    for (int f = 0; f < 10; f++) {
        g_ring.active_ring_slots[win_id][f] = -1;
    }

    S(g_wsi_state[win_id], 1);
}
