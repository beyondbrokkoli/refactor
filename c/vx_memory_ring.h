/*
   vx_memory_ring.h — Tier 1: Lock-free queues
*/
#pragma once
#include "vx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

EXPORT RenderPacket* vx_stream_packet(int idx);
EXPORT int           vx_stream_acquire(int win_id);
EXPORT void          vx_stream_commit(int win_id, int idx);
EXPORT void          vx_stream_init(int win_id, VulkanDeviceContext* dev_ctx);

#ifdef __cplusplus
}
#endif
