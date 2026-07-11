/*
   vx_vk_core.h — Tier 3: Vulkan foundation
*/
#pragma once
#include "vx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

EXPORT void vx_sys_inject_validation(void* instance_ptr);
EXPORT void vx_sys_eject_validation(void* instance);

#ifdef __cplusplus
}
#endif
