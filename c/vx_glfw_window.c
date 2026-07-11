/*
   vx_glfw_window.c — Tier 2: Window management
*/
#include "vx_glfw_window.h"

static bool s_is_fullscreen[MAX_WINDOWS] = {false};
static int  s_win_x[MAX_WINDOWS] = {0};
static int  s_win_y[MAX_WINDOWS] = {0};
static int  s_win_w[MAX_WINDOWS] = {1280};
static int  s_win_h[MAX_WINDOWS] = {720};

EXPORT const char** vx_sys_glfw_extensions(uint32_t* count) {
    return glfwGetRequiredInstanceExtensions(count);
}

EXPORT void vx_sys_publish_instance(int win_id, void* instance) {
    if (win_id < 0 || win_id >= MAX_WINDOWS) return;
    S(g_engine.mailbox.tenants[win_id].vk_instance, instance);
}

EXPORT void* vx_sys_get_surface(int win_id) {
    if (win_id < 0 || win_id >= MAX_WINDOWS) return NULL;
    return L(g_engine.mailbox.tenants[win_id].vk_surface);
}

EXPORT void vx_sys_set_cmd(int win_id, int cmd, int w, int h) {
    if (win_id < 0 || win_id >= MAX_WINDOWS) return;
    S_R(g_engine.mailbox.tenants[win_id].glfw_arg_w, w);
    S_R(g_engine.mailbox.tenants[win_id].glfw_arg_h, h);
    S(g_engine.mailbox.tenants[win_id].glfw_cmd, cmd);
}

EXPORT int vx_sys_get_cmd(int win_id) {
    if (win_id < 0 || win_id >= MAX_WINDOWS) return 0;
    return L(g_engine.mailbox.tenants[win_id].glfw_cmd);
}

EXPORT void vx_sys_window_size(int win_id, int* w, int* h) {
    if (win_id < 0 || win_id >= MAX_WINDOWS) {
        *w = 0; *h = 0;
        return;
    }
    *w = L(g_engine.mailbox.tenants[win_id].win_w);
    *h = L(g_engine.mailbox.tenants[win_id].win_h);
}

EXPORT int vx_sys_is_tenant_idle(int win_id) {
    if (win_id < 0 || win_id >= MAX_WINDOWS) return 1;

    int busy = L(g_render_busy[win_id]);
    int cmd = L(g_engine.mailbox.tenants[win_id].glfw_cmd);

    return (busy == 0 && cmd == CMD_IDLE) ? 1 : 0;
}
