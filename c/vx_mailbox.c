/*
   vx_mailbox.c — Tier 1: Mailbox lifecycle
*/
#include "vx_mailbox.h"

/* ── Global Variable Definitions ─────────────────────────────────────────── */
EngineState g_engine;
RenderRing g_ring;

VulkanDeviceContext    g_device_ctx[MAX_WINDOWS];
VulkanSwapchainContext g_wsi_ctx[MAX_WINDOWS][2];
_Atomic uint32_t       g_wsi_generation[MAX_WINDOWS];

atomic_int       g_wsi_state[MAX_WINDOWS];
atomic_int       g_render_busy[MAX_WINDOWS];
atomic_int       g_transfer_busy[MAX_WINDOWS];

vmath_thread_t   g_render_thread;
atomic_int       g_render_thread_active;

TransferJob      g_transfer_ring[TRANSFER_RING_SIZE];

vmath_thread_t   g_transfer_thread;
atomic_int       g_transfer_thread_active;

VkCommandPool    g_render_cmd_pools[MAX_WINDOWS];
VkCommandPool    g_transfer_cmd_pools[MAX_WINDOWS];
VkCommandBuffer  g_render_cmd_buffers[MAX_WINDOWS][3];
VkCommandBuffer  g_transfer_cmd_buffers[MAX_WINDOWS];
VkFence          g_render_fences[MAX_WINDOWS][3];
VkFence          g_transfer_fences[MAX_WINDOWS];

/* ── Mailbox Bootstrap ───────────────────────────────────────────────────── */
EXPORT void vx_init_mailbox(void) {
    atomic_init(&g_engine.mailbox.ready_index,  0);
    atomic_init(&g_engine.mailbox.is_running,   1);
    atomic_init(&g_engine.mailbox.lua_finished,  0);
    atomic_init(&g_engine.mailbox.active_window, 0);

    for (int i = 0; i < MAX_WINDOWS; i++) {
        atomic_init(&g_engine.mailbox.tenants[i].vk_instance,    NULL);
        atomic_init(&g_engine.mailbox.tenants[i].vk_surface,     NULL);
        atomic_init(&g_engine.mailbox.tenants[i].glfw_cmd,       CMD_IDLE);
        atomic_init(&g_engine.mailbox.tenants[i].glfw_arg_w,     0);
        atomic_init(&g_engine.mailbox.tenants[i].glfw_arg_h,     0);
        atomic_init(&g_engine.mailbox.tenants[i].last_key_pressed, 0);
        atomic_init(&g_engine.mailbox.tenants[i].wasd_mask,      0);
        atomic_init(&g_engine.mailbox.tenants[i].mouse_dx,       0.0f);
        atomic_init(&g_engine.mailbox.tenants[i].mouse_dy,       0.0f);
        atomic_init(&g_engine.mailbox.tenants[i].mouse_x,        0.0f);
        atomic_init(&g_engine.mailbox.tenants[i].mouse_y,        0.0f);
        atomic_init(&g_engine.mailbox.tenants[i].click_x,        -1.0f);
        atomic_init(&g_engine.mailbox.tenants[i].click_y,        -1.0f);
        atomic_init(&g_engine.mailbox.tenants[i].mouse_left,     0);
        atomic_init(&g_engine.mailbox.tenants[i].mouse_right,    0);
        atomic_init(&g_engine.mailbox.tenants[i].mouse_captured, 0);
        atomic_init(&g_engine.mailbox.tenants[i].window_resized, 0);
        atomic_init(&g_engine.mailbox.tenants[i].win_w,          1280);
        atomic_init(&g_engine.mailbox.tenants[i].win_h,          720);
        atomic_init(&g_engine.mailbox.tenants[i].key_space,      0);
    }

    atomic_init(&g_ring.locked_mask, 0);
    for (int i = 0; i < MAX_WINDOWS; i++) {
        atomic_init(&g_wsi_state[i], 0);
        atomic_init(&g_render_busy[i], 0);
        atomic_init(&g_transfer_busy[i], 0);
        atomic_init(&g_ring.ready_idx[i], -1);
        atomic_init(&g_ring.local_read[i], -1);
        for (int f = 0; f < 10; f++) {
            g_ring.active_ring_slots[i][f] = -1;
        }
    }
}

/* ── Lifecycle ───────────────────────────────────────────────────────────── */
EXPORT int vx_core_is_running(void) {
    return L_R(g_engine.mailbox.is_running);
}

EXPORT void vx_core_shutdown(void) {
    S(g_engine.mailbox.is_running, 0);
}

EXPORT void vx_core_mark_finished(void) {
    S(g_engine.mailbox.lua_finished, 1);
}
