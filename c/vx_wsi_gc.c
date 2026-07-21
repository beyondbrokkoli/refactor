/*
   vx_wsi_gc.c — Tier 4: Zombie garbage collection
*/
#include "vx_wsi_gc.h"

EXPORT void vx_pump_zombie_gc(void) {
    for (int wid = 0; wid < MAX_WINDOWS; wid++) {
        uint32_t active_gen = L(g_wsi_generation[wid]);
        uint32_t inactive_idx = (active_gen + 1) % 2;

        VulkanSwapchainContext* zombie = &g_wsi_ctx[wid][inactive_idx];
        uint32_t status = atomic_load_explicit((_Atomic uint32_t*)&zombie->status, memory_order_relaxed);

        if (status == 2) {
            VulkanDeviceContext* dev_ctx = &g_device_ctx[wid];

            // [THE VVL SCALPEL] Ask the GPU natively if the queue is still using these images!
            bool gpu_busy = false;
            for (int i = 0; i < 10; i++) {
                VkFence f = zombie->images_in_flight_fences[i];
                if (f != VK_NULL_HANDLE) {
                    if (vkGetFenceStatus(dev_ctx->device, f) == VK_NOT_READY) {
                        gpu_busy = true;
                        break;
                    }
                }
            }

            // If the GPU is still munching on the zombie frames, abort the GC.
            // We will check again on the next main loop iteration. No blocking!
            if (gpu_busy) {
                continue;
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
            printf("[C-CORE] Tenant %d: Swapchain safely garbage collected.\n", wid);
        }
    }
}
