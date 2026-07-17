/*
   vx_wsi_state.c — Tier 4: WSI state tracking
*/
#include "vx_wsi_state.h"

EXPORT uint32_t vx_sys_get_wsi_generation(int win_id) {
    if (win_id < 0 || win_id >= MAX_WINDOWS) return 0;
    return L(g_wsi_generation[win_id]);
}

EXPORT VulkanSwapchainContext* vx_sys_get_inactive_wsi_slot(int win_id) {
    if (win_id < 0 || win_id >= MAX_WINDOWS) return NULL;

    uint32_t active_gen = L(g_wsi_generation[win_id]);
    uint32_t inactive_idx = (active_gen + 1) % 2;

    return &g_wsi_ctx[win_id][inactive_idx];
}
