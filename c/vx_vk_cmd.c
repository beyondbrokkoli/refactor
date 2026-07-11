/*
   vx_vk_cmd.c — Tier 3: Command recording
*/
#include "vx_vk_cmd.h"

EXPORT void vx_stream_allocate_tenant(int wid, VulkanDeviceContext* dev_ctx,
                                      uint32_t gfx_family, uint32_t transfer_family) {
    if (wid < 0 || wid >= MAX_WINDOWS) return;
    if (!dev_ctx || !dev_ctx->device) return;

    if (g_render_cmd_pools[wid] == VK_NULL_HANDLE) {
        VkCommandPoolCreateInfo r_pool_info = {
            .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = gfx_family
        };
        vkCreateCommandPool(dev_ctx->device, &r_pool_info, NULL, &g_render_cmd_pools[wid]);

        VkCommandBufferAllocateInfo r_alloc_info = {
            .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool        = g_render_cmd_pools[wid],
            .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 3
        };
        vkAllocateCommandBuffers(dev_ctx->device, &r_alloc_info, g_render_cmd_buffers[wid]);
    }

    if (g_transfer_cmd_pools[wid] == VK_NULL_HANDLE) {
        VkCommandPoolCreateInfo t_pool_info = {
            .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = transfer_family
        };
        vkCreateCommandPool(dev_ctx->device, &t_pool_info, NULL, &g_transfer_cmd_pools[wid]);

        VkCommandBufferAllocateInfo t_alloc_info = {
            .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool        = g_transfer_cmd_pools[wid],
            .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };
        vkAllocateCommandBuffers(dev_ctx->device, &t_alloc_info, &g_transfer_cmd_buffers[wid]);

        VkFenceCreateInfo f_info = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT
        };
        for(int i = 0; i < 3; i++) {
            vkCreateFence(dev_ctx->device, &f_info, NULL, &g_render_fences[wid][i]);
        }

        VkFenceCreateInfo fence_info = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = 1
        };
        vkCreateFence(dev_ctx->device, &fence_info, NULL, &g_transfer_fences[wid]);
    }
}

EXPORT void vx_record_commands(VkCommandBuffer cmd, RenderPacket* p,
                               DrawCommand* queue, uint32_t count,
                               VulkanDeviceContext* dev_ctx,
                               uint64_t target_image, uint64_t target_view) {
    if (p->width == 0 || p->height == 0) return;

    VkCommandBufferBeginInfo beginInfo = { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    vkBeginCommandBuffer(cmd, &beginInfo);

    VkImageMemoryBarrier preBarriers[2] = {0};
    preBarriers[0].sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    preBarriers[0].oldLayout        = VK_IMAGE_LAYOUT_UNDEFINED;
    preBarriers[0].newLayout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    preBarriers[0].image            = (VkImage)target_image;
    preBarriers[0].subresourceRange = (VkImageSubresourceRange){ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    preBarriers[0].dstAccessMask    = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    preBarriers[1].sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    preBarriers[1].oldLayout        = VK_IMAGE_LAYOUT_UNDEFINED;
    preBarriers[1].newLayout        = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
    preBarriers[1].image            = (VkImage)p->depth_image;
    preBarriers[1].subresourceRange = (VkImageSubresourceRange){ VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1};
    preBarriers[1].dstAccessMask    = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        0, 0, NULL, 0, NULL, 2, preBarriers);

    PFN_vkCmdBeginRenderingKHR pfnBegin = (PFN_vkCmdBeginRenderingKHR)dev_ctx->pfnBegin;
    PFN_vkCmdEndRenderingKHR   pfnEnd   = (PFN_vkCmdEndRenderingKHR)dev_ctx->pfnEnd;

    VkRenderingAttachmentInfoKHR colorAttachment = {0};
    colorAttachment.sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
    colorAttachment.imageView   = (VkImageView)target_view;
    colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp     = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.clearValue.color.float32[0] = 0.01f;
    colorAttachment.clearValue.color.float32[1] = 0.01f;
    colorAttachment.clearValue.color.float32[2] = 0.02f;
    colorAttachment.clearValue.color.float32[3] = 1.0f;

    VkRenderingAttachmentInfoKHR depthAttachment = {
        .sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
        .imageView   = (VkImageView)p->depth_view,
        .imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        .loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp     = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue.depthStencil = {1.0f, 0}
    };

    VkRenderingInfoKHR renderInfo = {
        .sType                = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
        .renderArea.extent    = {p->width, p->height},
        .layerCount           = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments    = &colorAttachment,
        .pDepthAttachment     = &depthAttachment
    };

    pfnBegin(cmd, &renderInfo);

    VkViewport viewport = {0.0f, 0.0f, (float)p->width, (float)p->height, 0.0f, 1.0f};
    VkRect2D   scissor  = {{0, 0}, {p->width, p->height}};
    vkCmdSetViewport(cmd, 0, 1, &viewport);
    vkCmdSetScissor(cmd, 0, 1, &scissor);

    if (count > 0 && p->vertex_buffer != 0 && p->index_buffer != 0) {
        VkDeviceSize offset = 0;
        VkBuffer vbo = (VkBuffer)p->vertex_buffer;
        vkCmdBindVertexBuffers(cmd, 0, 1, &vbo, &offset);

        VkBuffer ibo = (VkBuffer)p->index_buffer;
        vkCmdBindIndexBuffer(cmd, ibo, 0, VK_INDEX_TYPE_UINT32);

        PFN_vkCmdSetCullModeEXT vkCmdSetCullModeEXT = (PFN_vkCmdSetCullModeEXT)dev_ctx->pfnSetCullMode;
        PFN_vkCmdSetFrontFaceEXT vkCmdSetFrontFaceEXT = (PFN_vkCmdSetFrontFaceEXT)dev_ctx->pfnSetFrontFace;
        PFN_vkCmdSetPrimitiveTopologyEXT vkCmdSetPrimitiveTopologyEXT = (PFN_vkCmdSetPrimitiveTopologyEXT)dev_ctx->pfnSetPrimitiveTopology;
        PFN_vkCmdSetDepthTestEnableEXT vkCmdSetDepthTestEnableEXT = (PFN_vkCmdSetDepthTestEnableEXT)dev_ctx->pfnSetDepthTestEnable;
        PFN_vkCmdSetDepthWriteEnableEXT vkCmdSetDepthWriteEnableEXT = (PFN_vkCmdSetDepthWriteEnableEXT)dev_ctx->pfnSetDepthWriteEnable;
        PFN_vkCmdSetDepthCompareOpEXT vkCmdSetDepthCompareOpEXT = (PFN_vkCmdSetDepthCompareOpEXT)dev_ctx->pfnSetDepthCompareOp;

        uint64_t current_pipeline   = 0;
        uint64_t current_descriptor = 0;

        #define MAX_DRAW_COMMANDS 1024
        uint32_t safe_count = count > MAX_DRAW_COMMANDS ? MAX_DRAW_COMMANDS : count;

        for (uint32_t i = 0; i < safe_count; i++) {
            DrawCommand* draw = &queue[i];

            if (draw->pipeline_id != current_pipeline) {
                vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, (VkPipeline)draw->pipeline_id);
                current_pipeline = draw->pipeline_id;
            }

            if (draw->descriptor_set != current_descriptor) {
                VkDescriptorSet dset = (VkDescriptorSet)draw->descriptor_set;
                vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    (VkPipelineLayout)p->gfx_layout, 0, 1, &dset, 0, NULL);
                current_descriptor = draw->descriptor_set;
            }

            VkRect2D dyn_scissor = {
                .offset = {(int32_t)draw->scissor_x, (int32_t)draw->scissor_y},
                .extent = {(uint32_t)draw->scissor_w, (uint32_t)draw->scissor_h}
            };
            vkCmdSetScissor(cmd, 0, 1, &dyn_scissor);

            vkCmdSetCullModeEXT(cmd,           draw->cull_mode);
            vkCmdSetFrontFaceEXT(cmd,          draw->front_face);
            vkCmdSetPrimitiveTopologyEXT(cmd,  draw->topology);
            vkCmdSetDepthTestEnableEXT(cmd,    draw->depth_test);
            vkCmdSetDepthWriteEnableEXT(cmd,   draw->depth_write);
            vkCmdSetDepthCompareOpEXT(cmd,     draw->depth_compare_op);

            vkCmdPushConstants(cmd, (VkPipelineLayout)p->gfx_layout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                draw->pc_offset, draw->pc_size, draw->push_constants + draw->pc_offset);

            vkCmdDrawIndexed(cmd, draw->index_count, draw->instance_count,
                             draw->first_index, draw->vertex_offset, draw->first_instance);
        }
    }

    pfnEnd(cmd);

    VkImageMemoryBarrier presentBarrier = {
        .sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .oldLayout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .newLayout        = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .image            = (VkImage)target_image,
        .subresourceRange = (VkImageSubresourceRange){ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
        .srcAccessMask    = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dstAccessMask    = 0
    };
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        0, 0, NULL, 0, NULL, 1, &presentBarrier);

    vkEndCommandBuffer(cmd);
}

EXPORT void vx_thread_kill(void) {
    S(g_render_thread_active,   0);
    S(g_transfer_thread_active, 0);

    vmath_thread_join(g_render_thread);
    vmath_thread_join(g_transfer_thread);

    for (int i = 0; i < MAX_WINDOWS; i++) {
        S(g_ring.ready_idx[i], -1);
    }
    S(g_ring.locked_mask, 0);

    for (int i = 0; i < MAX_WINDOWS; i++) {
        if (g_device_ctx[i].device) {
            vkDeviceWaitIdle(g_device_ctx[i].device);
        }
        if (g_render_cmd_pools[i]) {
            vkDestroyCommandPool(g_device_ctx[i].device, g_render_cmd_pools[i], NULL);
            g_render_cmd_pools[i] = VK_NULL_HANDLE;
        }
        for (int j = 0; j < 3; j++) {
            if (g_render_fences[i][j] != VK_NULL_HANDLE) {
                vkDestroyFence(g_device_ctx[i].device, g_render_fences[i][j], NULL);
                g_render_fences[i][j] = VK_NULL_HANDLE;
            }
        }
        if (g_transfer_cmd_pools[i]) {
            vkDestroyCommandPool(g_device_ctx[i].device, g_transfer_cmd_pools[i], NULL);
            g_transfer_cmd_pools[i] = VK_NULL_HANDLE;
        }
        if (g_transfer_fences[i]) {
            vkDestroyFence(g_device_ctx[i].device, g_transfer_fences[i], NULL);
            g_transfer_fences[i] = VK_NULL_HANDLE;
        }
    }

    printf("[C-CORE] Async Threads joined, Devices idled, Ring Purged, and Pools/Fences destroyed.\n");
}
