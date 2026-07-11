/*
   vx_vk_cmd.h — Tier 3: Command recording
*/
#pragma once
#include "vx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

EXPORT void vx_stream_allocate_tenant(int wid, VulkanDeviceContext* dev_ctx,
                                      uint32_t gfx_family, uint32_t transfer_family);
EXPORT void vx_record_commands(VkCommandBuffer cmd, RenderPacket* p,
                               DrawCommand* queue, uint32_t count,
                               VulkanDeviceContext* dev_ctx,
                               uint64_t target_image, uint64_t target_view);
EXPORT void vx_thread_kill(void);

#ifdef __cplusplus
}
#endif
