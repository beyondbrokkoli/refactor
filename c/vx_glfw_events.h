/*
   vx_glfw_events.h — Tier 2: Window events
*/
#pragma once
#include "vx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height);
EXPORT int vx_sys_get_resize_state(int win_id);

#ifdef __cplusplus
}
#endif
