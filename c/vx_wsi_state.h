/*
   vx_wsi_state.h — Tier 4: WSI state tracking
*/
#pragma once
#include "vx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

EXPORT uint32_t vx_sys_get_wsi_generation(int win_id);
EXPORT VulkanSwapchainContext* vx_sys_get_inactive_wsi_slot(int win_id);
EXPORT uint64_t vx_sys_get_timeline_counter(int win_id);
EXPORT VulkanSwapchainContext* vx_sys_get_active_wsi_slot(int win_id);

#ifdef __cplusplus
}
#endif
