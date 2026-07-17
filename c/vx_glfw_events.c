/*
   vx_glfw_events.c — Tier 2: Window events
*/
#include "vx_glfw_events.h"

void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    if (width == 0 || height == 0) return;

    int id = (int)(intptr_t)glfwGetWindowUserPointer(window);
    if (id < 0 || id >= MAX_WINDOWS) return;

    S(g_engine.mailbox.tenants[id].win_w, width);
    S(g_engine.mailbox.tenants[id].win_h, height);
    S(g_engine.mailbox.tenants[id].window_resized, 1);
}

EXPORT int vx_sys_get_resize_state(int win_id) {
    if (win_id < 0 || win_id >= MAX_WINDOWS) return 0;

    // THE FIX: Atomically swap the flag back to 0 when Lua reads it.
    // If it was 1, Lua gets 1 and it becomes 0.
    return atomic_exchange_explicit(&g_engine.mailbox.tenants[win_id].window_resized, 0, memory_order_acquire);
}
