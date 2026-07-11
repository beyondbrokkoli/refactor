/*
   vx_glfw_input.h — Tier 2: Input callbacks
*/
#pragma once
#include "vx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void glfw_cursor_callback(GLFWwindow* window, double xpos, double ypos);
void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

EXPORT int        vx_input_last_key(int win_id);
EXPORT int        vx_input_get_active_window(void);
EXPORT int        vx_input_mouse_btn(int win_id, int btn);
EXPORT float      vx_input_mouse_x(int win_id);
EXPORT float      vx_input_mouse_y(int win_id);
EXPORT float      vx_input_click_x(int win_id);
EXPORT float      vx_input_click_y(int win_id);
EXPORT int        vx_input_is_captured(int win_id);
EXPORT uint32_t   vx_input_wasd(int win_id);
EXPORT float      vx_input_mouse_dx(int win_id);
EXPORT float      vx_input_mouse_dy(int win_id);
EXPORT int        vx_input_spacebar(int win_id);

#ifdef __cplusplus
}
#endif
