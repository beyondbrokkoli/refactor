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

// NEW EXPORT: Safely read the standard uint64_t as an atomic
EXPORT uint64_t vx_sys_get_timeline_counter(int win_id) {
    if (win_id < 0 || win_id >= MAX_WINDOWS) return 0;
    return atomic_load_explicit((_Atomic uint64_t*)&g_device_ctx[win_id].cpu_timeline_counter, memory_order_acquire);
}

// NEW EXPORT: Grab the active slot so Lua can tag it before the flip
EXPORT VulkanSwapchainContext* vx_sys_get_active_wsi_slot(int win_id) {
    if (win_id < 0 || win_id >= MAX_WINDOWS) return NULL;
    uint32_t active_gen = L(g_wsi_generation[win_id]);
    return &g_wsi_ctx[win_id][active_gen % 2];
}
