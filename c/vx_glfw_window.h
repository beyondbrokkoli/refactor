/*
   vx_glfw_window.h — Tier 2: Window management
*/
#pragma once
#include "vx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

EXPORT const char** vx_sys_glfw_extensions(uint32_t* count);
EXPORT void         vx_sys_publish_instance(int win_id, void* instance);
EXPORT void*        vx_sys_get_surface(int win_id);
EXPORT void         vx_sys_set_cmd(int win_id, int cmd, int w, int h);
EXPORT int          vx_sys_get_cmd(int win_id);
EXPORT void         vx_sys_window_size(int win_id, int* w, int* h);
EXPORT int          vx_sys_is_tenant_idle(int win_id);

#ifdef __cplusplus
}
#endif
