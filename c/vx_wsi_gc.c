/*
   vx_wsi_gc.c — Tier 4: Zombie garbage collection
*/
#include "vx_wsi_gc.h"

typedef VkResult (*PFN_vkWaitForPresentKHR)(VkDevice device, VkSwapchainKHR swapchain, uint64_t presentId, uint64_t timeout);

EXPORT void vx_pump_zombie_gc(void) {
    for (int wid = 0; wid < MAX_WINDOWS; wid++) {
        uint32_t active_gen = L(g_wsi_generation[wid]);
        uint32_t inactive_idx = (active_gen + 1) % 2;
        uint32_t active_idx = active_gen % 2;

        VulkanSwapchainContext* zombie = &g_wsi_ctx[wid][inactive_idx];
        VulkanSwapchainContext* active_wsi = &g_wsi_ctx[wid][active_idx];

        uint32_t status = atomic_load_explicit((_Atomic uint32_t*)&zombie->status, memory_order_relaxed);

        if (status == 2) {
            VulkanDeviceContext* dev_ctx = &g_device_ctx[wid];

            // [THE PROXY WAIT]
            if (active_wsi->swapchain != VK_NULL_HANDLE && dev_ctx->pfnWaitForPresentKHR) {
                extern int vx_sys_get_resize_state(int win_id);

                // 1. [THE HOISTED SHOCK ABSORBER]
                // If X11 is actively tearing the geometry, the active WSI is compromised.
                // Its frames might be swallowed by the void. We cannot trust the compositor
                // to signal presentId 2. Sync the queue and smash the zombie.
                if (vx_sys_get_resize_state(wid) == 1) {
                    vkQueueWaitIdle(dev_ctx->queue);
                }
                // 2. Window is stable. We can safely trust the OS Compositor handshake.
                else {
                    PFN_vkWaitForPresentKHR pfnWaitPresent = (PFN_vkWaitForPresentKHR)dev_ctx->pfnWaitForPresentKHR;
                    uint64_t current_present_id = atomic_load_explicit(
                        (_Atomic uint64_t*)&active_wsi->present_id_counter, memory_order_acquire);

                    if (current_present_id >= 2) {
                        // [TIMEOUT 0]: Non-blocking query
                        VkResult res = pfnWaitPresent(dev_ctx->device, active_wsi->swapchain, 2, 0);
                        if (res == VK_TIMEOUT) {
                            continue; // OS still processing. Poll again next loop.
                        }
                    } else {
                        // Stable window, but active WSI hasn't rendered 2 frames yet (minimized/paused).
                        continue;
                    }
                }
            }

            for (int i = 0; i < 10; i++) {
                if (zombie->swapchain_views[i] != 0) {
                    vkDestroyImageView(dev_ctx->device, (VkImageView)zombie->swapchain_views[i], NULL);
                    zombie->swapchain_views[i] = 0;
                }
            }

            if (zombie->swapchain != VK_NULL_HANDLE) {
                vkDestroySwapchainKHR(dev_ctx->device, zombie->swapchain, NULL);
                zombie->swapchain = VK_NULL_HANDLE;
            }

            atomic_store_explicit((_Atomic uint32_t*)&zombie->status, 0, memory_order_release);
            printf("[C-CORE] Tenant %d: Swapchain safely garbage collected via Proxy Wait.\n", wid);
        }
    }
}
