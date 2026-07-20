/*
   vx_multiplexer.c — Tier 5: Render orchestrator
   The beast: render_thread_loop, transfer_thread_loop, and WSI command orchestration.
*/
#include "vx_multiplexer.h"
#include "vx_thread_utils.h"
#include "vx_vk_cmd.h"

/* Forward declarations for thread loops */
static THREAD_FUNC render_thread_loop(void* arg);
static THREAD_FUNC transfer_thread_loop(void* arg);

EXPORT void vx_transfer_setup(void) {
    for (int i = 0; i < TRANSFER_RING_SIZE; i++) {
        atomic_init(&g_transfer_ring[i].status, 0);
    }
}

EXPORT int vx_transfer_request(int win_id, uint64_t src, uint64_t dst,
                               uint64_t size, uint64_t t_sem, uint64_t sig_val) {
    if (win_id < 0 || win_id >= MAX_WINDOWS) return 0;

    for (int i = 0; i < TRANSFER_RING_SIZE; i++) {
        int expected = 0;
        if (CXS(g_transfer_ring[i].status, expected, 1)) {
            g_transfer_ring[i].src_buffer       = src;
            g_transfer_ring[i].dst_buffer       = dst;
            g_transfer_ring[i].size             = size;
            g_transfer_ring[i].timeline_sem     = t_sem;
            g_transfer_ring[i].signal_val       = sig_val;
            g_transfer_ring[i].target_window_id = win_id;
            S(g_transfer_ring[i].status, 2);
            return 1;
        }
    }
    return 0;
}

static THREAD_FUNC transfer_thread_loop(void* arg) {
    printf("[C-CORE] Async Transfer Overlord Online.\n");

    while (L(g_transfer_thread_active) && L(g_engine.mailbox.is_running)) {
        bool worked = false;

        for (int i = 0; i < TRANSFER_RING_SIZE; i++) {
            if (L(g_transfer_ring[i].status) == 2) {
                TransferJob* job = &g_transfer_ring[i];
                int wid = job->target_window_id;

                S(g_transfer_busy[wid], 1);

                if (wid < 0 || wid >= MAX_WINDOWS || L(g_wsi_state[wid]) == 0) {
                    S(job->status, 0);
                    S(g_transfer_busy[wid], 0);
                    continue;
                }

                VulkanDeviceContext* dev_ctx = &g_device_ctx[wid];
                if (!dev_ctx->device || g_transfer_cmd_pools[wid] == VK_NULL_HANDLE) {
                    S(job->status, 0);
                    S(g_transfer_busy[wid], 0);
                    continue;
                }

                VkCommandBuffer cmd   = g_transfer_cmd_buffers[wid];
                VkFence         fence = g_transfer_fences[wid];

                PFN_vkWaitForFences pfnWait   = (PFN_vkWaitForFences)dev_ctx->vkWaitForFences;
                PFN_vkResetFences   pfnReset  = (PFN_vkResetFences)dev_ctx->vkResetFences;
                PFN_vkQueueSubmit   pfnSubmit = (PFN_vkQueueSubmit)dev_ctx->vkQueueSubmit;

                VkResult res = pfnWait(dev_ctx->device, 1, &fence, VK_TRUE, 2000000000);
                if (res == VK_TIMEOUT) {
                    printf("[C-FATAL] Tenant %d: GPU Transfer Hang detected!\n", wid);
                    S(job->status, 0);
                    S(g_wsi_state[wid], 0);
                    S(g_transfer_busy[wid], 0);
                    break;
                }

                pfnReset(dev_ctx->device, 1, &fence);
                vkResetCommandBuffer(cmd, 0);

                VkCommandBufferBeginInfo beginInfo = {
                    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
                };
                vkBeginCommandBuffer(cmd, &beginInfo);

                VkBufferCopy copyRegion = {
                    .srcOffset = 0, .dstOffset = 0, .size = job->size
                };
                vkCmdCopyBuffer(cmd, (VkBuffer)job->src_buffer, (VkBuffer)job->dst_buffer, 1, &copyRegion);
                vkEndCommandBuffer(cmd);

                VkSemaphore safe_timeline_semaphore = (VkSemaphore)(uintptr_t)job->timeline_sem;

                VkTimelineSemaphoreSubmitInfo timelineInfo = {
                    .sType                  = 1000207003,
                    .signalSemaphoreValueCount = 1,
                    .pSignalSemaphoreValues = &job->signal_val
                };

                VkSubmitInfo submitInfo = {
                    .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                    .pNext                = &timelineInfo,
                    .commandBufferCount   = 1,
                    .pCommandBuffers      = &cmd,
                    .signalSemaphoreCount = 1,
                    .pSignalSemaphores    = &safe_timeline_semaphore
                };

                pfnSubmit(dev_ctx->transfer_queue, 1, &submitInfo, fence);
                pfnWait(dev_ctx->device, 1, &fence, VK_TRUE, 2000000000);

                S(job->status, 0);
                S(g_transfer_busy[wid], 0);
                worked = true;
            }
        }

        if (!worked) SLEEP_MS(1);
    }

    printf("[C-CORE] Async Transfer Thread gracefully terminated.\n");
    return NULL;
}

static THREAD_FUNC render_thread_loop(void* arg) {
    printf("[C-CORE] Async Render Multiplexer Online.\n");

    uint32_t t_frame[MAX_WINDOWS] = {0};

    while (L(g_render_thread_active) && L(g_engine.mailbox.is_running)) {

        for (int w = 0; w < MAX_WINDOWS; w++) {
            int cmd = atomic_load_explicit(&g_engine.mailbox.tenants[w].glfw_cmd, memory_order_acquire);

            if (cmd == CMD_REBUILD_WSI) {
                int timeout = 2000;
                int spin_count = 0;
                while (L(g_transfer_busy[w]) && timeout > 0) {
                    if (spin_count >= 2000) { timeout--; }
                    vx_spin_wait(&spin_count);
                }

                VulkanDeviceContext* dev_ctx = &g_device_ctx[w];
                if (dev_ctx->device) {
                    if (dev_ctx->queue) {
                        vkQueueWaitIdle(dev_ctx->queue);
                    }
                    if (dev_ctx->transfer_queue) {
                        vkQueueWaitIdle(dev_ctx->transfer_queue);
                    }

                    if (g_render_cmd_pools[w]) {
                       vkResetCommandPool(dev_ctx->device, g_render_cmd_pools[w], 0);
                    }

                    if (g_transfer_cmd_pools[w]) {
                        vkResetCommandPool(dev_ctx->device, g_transfer_cmd_pools[w], 0);
                    }
                }

                S(g_wsi_state[w], 0);
                atomic_store_explicit(&g_engine.mailbox.tenants[w].window_resized, 0, memory_order_release);
                atomic_store_explicit(&g_engine.mailbox.tenants[w].glfw_cmd, CMD_IDLE, memory_order_release);
            }
        }

        for (int wid = 0; wid < MAX_WINDOWS; wid++) {
            int ready          = L(g_ring.ready_idx[wid]);
            int local_read_val = L(g_ring.local_read[wid]);

            if (ready == -1 || ready == local_read_val) {
                continue;
            }

            int offset = wid * 4;
            int new_local_read;

            if (local_read_val == -1) {
                new_local_read = offset + 0;
            } else {
                int curr_local = local_read_val - offset;
                new_local_read = offset + ((curr_local + 1) % 4);
            }

            S(g_ring.local_read[wid], new_local_read);
            int read_idx = new_local_read;

            RenderPacket* p = &g_ring.packets[read_idx];
            S(g_render_busy[wid], 1);

            uint32_t gen = p->swapchain_generation;
            uint32_t current_gen = L(g_wsi_generation[wid]);

            // [THE FIX] Drop packets that belong to a zombie swapchain
            if (gen != current_gen) {
                // We don't want to spam the console, but a debug print helps confirm it works:
                // printf("[C-CORE] Tenant %d: Dropping stale packet (Gen %d vs Current %d)\n", wid, gen, current_gen);
                goto frame_done;
            }

            uint32_t active_idx = gen & 1;

            VulkanDeviceContext* dev_ctx = &g_device_ctx[wid];
            VulkanSwapchainContext* win_wsi = &g_wsi_ctx[wid][active_idx];

            uint32_t frame_slots = dev_ctx->max_frames_in_flight > 0 ? dev_ctx->max_frames_in_flight : 3;
            if (frame_slots > 3) frame_slots = 3;
            uint32_t current_frame = t_frame[wid] % frame_slots;

            VkFence immortal_fence = g_render_fences[wid][current_frame];
            PFN_vkWaitForFences pfnWait = (PFN_vkWaitForFences)dev_ctx->vkWaitForFences;
            if (pfnWait(dev_ctx->device, 1, &immortal_fence, VK_TRUE, 2000000000) == VK_TIMEOUT) {
                printf("[C-WARN] Tenant %d: GPU Fence Timeout (CPU Starvation).\n", wid);
                goto frame_done;
            }

            int finished_slot = g_ring.active_ring_slots[wid][current_frame];
            if (finished_slot != -1 && finished_slot != read_idx) {
                FA(g_ring.locked_mask, ~(1u << finished_slot));
            }
            g_ring.active_ring_slots[wid][current_frame] = read_idx;

            uint32_t w_status = atomic_load_explicit((_Atomic uint32_t*)&win_wsi->status, memory_order_acquire);

            if (win_wsi->swapchain == VK_NULL_HANDLE || L(g_wsi_state[wid]) == 0 ||
                p->width == 0 || p->height == 0 || w_status != 1) {
                goto frame_done;
            }

            VkCommandBuffer cmd_buf = g_render_cmd_buffers[wid][current_frame];

            PFN_vkAcquireNextImageKHR pfnAcquire = (PFN_vkAcquireNextImageKHR)dev_ctx->vkAcquireNextImageKHR;
            uint32_t img_idx;
            VkResult res = pfnAcquire(dev_ctx->device, win_wsi->swapchain,
                5000000, win_wsi->image_available[current_frame], VK_NULL_HANDLE, &img_idx);

            if (res == VK_TIMEOUT || res == VK_NOT_READY) goto frame_done;
            if (res == VK_ERROR_OUT_OF_DATE_KHR) {
                S(g_engine.mailbox.tenants[wid].window_resized, 1);
                SLEEP_MS(10);
                goto frame_done;
            }

            if (win_wsi->images_in_flight_fences[img_idx] != VK_NULL_HANDLE) {
                pfnWait(dev_ctx->device, 1, &win_wsi->images_in_flight_fences[img_idx], VK_TRUE, 2000000000);
            }
            win_wsi->images_in_flight_fences[img_idx] = immortal_fence;

            PFN_vkResetFences pfnReset = (PFN_vkResetFences)dev_ctx->vkResetFences;
            pfnReset(dev_ctx->device, 1, &immortal_fence);

            uint64_t local_image = win_wsi->swapchain_images[img_idx];
            uint64_t local_view  = win_wsi->swapchain_views[img_idx];

            vkResetCommandBuffer(cmd_buf, 0);

            vx_record_commands(cmd_buf, p, p->draw_queue, p->draw_count, dev_ctx, local_image, local_view);

            VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

            VkSemaphore render_finished_sem = win_wsi->render_finished[img_idx];

            VkSubmitInfo submitInfo = {
                .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .waitSemaphoreCount   = 1,
                .pWaitSemaphores      = &win_wsi->image_available[current_frame],
                .pWaitDstStageMask    = &waitStage,
                .commandBufferCount   = 1,
                .pCommandBuffers      = &cmd_buf,
                .signalSemaphoreCount = 1,
                .pSignalSemaphores    = &render_finished_sem
            };

            PFN_vkQueueSubmit pfnSubmit = (PFN_vkQueueSubmit)dev_ctx->vkQueueSubmit;
            pfnSubmit(dev_ctx->queue, 1, &submitInfo, immortal_fence);

            VkPresentInfoKHR presentInfo = {
                .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores    = &render_finished_sem,
                .swapchainCount     = 1,
                .pSwapchains        = &win_wsi->swapchain,
                .pImageIndices      = &img_idx
            };

            PFN_vkQueuePresentKHR pfnPresent = (PFN_vkQueuePresentKHR)dev_ctx->vkQueuePresentKHR;
            pfnPresent(dev_ctx->queue, &presentInfo);

        frame_done:
            uint32_t current_active_gen = L(g_wsi_generation[wid]);
            uint32_t inactive_idx = (current_active_gen + 1) % 2;
            VulkanSwapchainContext* zombie = &g_wsi_ctx[wid][inactive_idx];

            uint32_t z_status = atomic_load_explicit((_Atomic uint32_t*)&zombie->status, memory_order_relaxed);
            if (z_status > 3) {
                atomic_fetch_sub_explicit((_Atomic uint32_t*)&zombie->status, 1, memory_order_relaxed);
            } else if (z_status == 3) {
                // [THE MATRIX DODGE] We are IN the render thread. We own this queue.
                // Idling it here takes < 1ms, avoids all collisions, and silences VVL forever.
                if (dev_ctx->queue) {
                    vkQueueWaitIdle(dev_ctx->queue);
                }
                atomic_store_explicit((_Atomic uint32_t*)&zombie->status, 2, memory_order_release);
            }

            S(g_render_busy[wid], 0);
            t_frame[wid] = (current_frame + 1) % frame_slots;
        }

        SLEEP_MS(1);
    }

    printf("[C-CORE] Async Render Multiplexer gracefully terminated.\n");
    return NULL;
}

EXPORT void vx_thread_start(void) {
    S(g_render_thread_active,   1);
    S(g_transfer_thread_active, 1);
    g_render_thread   = vmath_thread_start(render_thread_loop,   NULL);
    g_transfer_thread = vmath_thread_start(transfer_thread_loop, NULL);
}
