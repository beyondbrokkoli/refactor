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

        // 1. CPU-SIDE YIELD: Give the async thread ~20 ticks to lock, submit,
        // and transition the fences from VK_SUCCESS to VK_NOT_READY.
        if (status > 100 && status <= 120) {
            atomic_store_explicit((_Atomic uint32_t*)&zombie->status, status - 1, memory_order_release);
            continue;
        }

        // 2. GPU MATHEMATICAL HANDSHAKE: The CPU is guaranteed done. Now we ask the GPU.
        if (status == 100) {
            VulkanDeviceContext* dev_ctx = &g_device_ctx[wid];
            bool gpu_is_finished = true;

            for (int i = 0; i < 10; i++) {
                if (zombie->in_flight[i] != VK_NULL_HANDLE) {
                    VkResult f_status = vkGetFenceStatus(dev_ctx->device, (VkFence)zombie->in_flight[i]);

                    if (f_status == VK_NOT_READY) {
                        gpu_is_finished = false;
                        break; // GPU is still rendering. Try again next tick.
                    }
                }
            }

            if (!gpu_is_finished) {
                continue;
            }

            // 3. SAFE DESTRUCTION ZONE
            for (int i = 0; i < 10; i++) {
                if (zombie->swapchain_views[i] != 0) {
                    vkDestroyImageView(dev_ctx->device, (VkImageView)zombie->swapchain_views[i], NULL);
                    zombie->swapchain_views[i] = 0;
                }
            }

            for (int i = 0; i < 10; i++) {
                if (zombie->image_available[i] != VK_NULL_HANDLE) {
                    vkDestroySemaphore(dev_ctx->device, zombie->image_available[i], NULL);
                    zombie->image_available[i] = VK_NULL_HANDLE;
                }
                if (zombie->render_finished[i] != VK_NULL_HANDLE) {
                    vkDestroySemaphore(dev_ctx->device, zombie->render_finished[i], NULL);
                    zombie->render_finished[i] = VK_NULL_HANDLE;
                }
                if (zombie->in_flight[i] != VK_NULL_HANDLE) {
                    vkDestroyFence(dev_ctx->device, (VkFence)zombie->in_flight[i], NULL);
                    zombie->in_flight[i] = VK_NULL_HANDLE;
                }
            }

            if (zombie->swapchain != VK_NULL_HANDLE) {
                vkDestroySwapchainKHR(dev_ctx->device, zombie->swapchain, NULL);
                zombie->swapchain = VK_NULL_HANDLE;
            }

            atomic_store_explicit((_Atomic uint32_t*)&zombie->status, 0, memory_order_release);
            printf("[C-CORE] Tenant %d: Zombie Swapchain (Slot %d) mathematically garbage collected.\n", wid, inactive_idx);
        }
    }
}
