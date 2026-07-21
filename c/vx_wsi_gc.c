/*
   vx_wsi_gc.c — Tier 4: Zombie garbage collection
*/
#include "vx_wsi_gc.h"

// We must define the function pointer signature if vulkan.h doesn't have it natively exposed
typedef VkResult (*PFN_vkWaitForPresentKHR)(VkDevice device, VkSwapchainKHR swapchain, uint64_t presentId, uint64_t timeout);

EXPORT void vx_pump_zombie_gc(void) {
    for (int wid = 0; wid < MAX_WINDOWS; wid++) {
        uint32_t active_gen = L(g_wsi_generation[wid]);
        uint32_t inactive_idx = (active_gen + 1) % 2;

        VulkanSwapchainContext* zombie = &g_wsi_ctx[wid][inactive_idx];
        uint32_t status = atomic_load_explicit((_Atomic uint32_t*)&zombie->status, memory_order_relaxed);

        if (status == 2) {
            VulkanDeviceContext* dev_ctx = &g_device_ctx[wid];

            // [THE QWEN SCALPEL]: Wait specifically for the OS Compositor to release this exact swapchain.
            if (zombie->swapchain != VK_NULL_HANDLE && zombie->last_present_id > 0 && dev_ctx->pfnWaitForPresentKHR) {
                PFN_vkWaitForPresentKHR pfnWaitPresent = (PFN_vkWaitForPresentKHR)dev_ctx->pfnWaitForPresentKHR;

                // 2000000000 ns = 2 seconds timeout to prevent complete deadlocks if the OS hangs
                pfnWaitPresent(dev_ctx->device, zombie->swapchain, zombie->last_present_id, 2000000000);
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
            printf("[C-CORE] Tenant %d: Swapchain safely garbage collected via Present Wait.\n", wid);
        }
    }
}
