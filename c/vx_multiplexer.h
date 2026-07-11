/*
   vx_multiplexer.h — Tier 5: Render orchestrator
*/
#pragma once
#include "vx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

EXPORT void vx_transfer_setup(void);
EXPORT int  vx_transfer_request(int win_id, uint64_t src, uint64_t dst,
                                uint64_t size, uint64_t t_sem, uint64_t sig_val);
EXPORT void vx_thread_start(void);

#ifdef __cplusplus
}
#endif
