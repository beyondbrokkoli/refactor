/*
   vx_mailbox.h — Tier 1: Mailbox lifecycle
*/
#pragma once
#include "vx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

EXPORT void vx_init_mailbox(void);
EXPORT int  vx_core_is_running(void);
EXPORT void vx_core_shutdown(void);
EXPORT void vx_core_mark_finished(void);

#ifdef __cplusplus
}
#endif
