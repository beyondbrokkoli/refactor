/*
   vx_types.h — Tier 0: The Data Bedrock
   Every single struct, #define, and typedef. No logic.
   This replaces shared_structs.h and provides the foundation for all other files.
*/
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <stdalign.h>

#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#endif

/* ── Vulkan + GLFW (Types required for struct definitions) */
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

/* ── Platform / Threading ────────────────────────────────────────────────── */
#include <pthread.h>
#include <unistd.h>
#include <sched.h>

#define SLEEP_MS(ms) usleep((ms) * 1000)

#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <timeapi.h>
#pragma comment(lib, "winmm.lib")
#endif

typedef pthread_t vmath_thread_t;
#define THREAD_FUNC        void*
#define THREAD_RETURN_VAL NULL

#if defined(_WIN32)
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __attribute__((visibility("default")))
#endif

/* ── Atomic Convenience Macros ───────────────────────────────────────────── */
#undef LOAD
#undef STORE

#define L_R(v, ...)   atomic_load_explicit(&(v), memory_order_relaxed)
#define L(v)          atomic_load_explicit(&(v), memory_order_acquire)
#define S_R(v, x)     atomic_store_explicit(&(v), (x), memory_order_relaxed)
#define S(v, x)       atomic_store_explicit(&(v), (x), memory_order_release)
#define E_R(v, x)     atomic_exchange_explicit(&(v), (x), memory_order_relaxed)
#define E_A(v, x)     atomic_exchange_explicit(&(v), (x), memory_order_acquire)
#define FO(v, x)      atomic_fetch_or_explicit(&(v), (x), memory_order_release)
#define FA(v, x)      atomic_fetch_and_explicit(&(v), (x), memory_order_release)
#define CWX(v, e, d)  atomic_compare_exchange_weak_explicit(&(v), &(e), (d), \
                          memory_order_release, memory_order_relaxed)
#define CXS(v, e, d)  atomic_compare_exchange_strong_explicit(&(v), &(e), (d), \
                          memory_order_acquire, memory_order_relaxed)
#define TAS(v)        atomic_flag_test_and_set_explicit(&(v), memory_order_acquire)
#define CLR(v)        atomic_flag_clear_explicit(&(v), memory_order_release)

/* ── Engine Constants ────────────────────────────────────────────────────── */
#define MODE_DUAL 0
#define MODE_GEOM 1
#define MODE_POINT_CLOUD_PASS 88
#define MODE_POINTS 2
#define FRAME_STATE_CONFIRMED 2
#define FRAME_STATE_EMPTY 0
#define FRAME_STATE_PREDICTED 1
#define WORLD_GRID_CELLS 262144
#define WORLD_MAP_HEIGHT 256
#define WORLD_MAP_WIDTH 256
#define WORLD_OFFSET_X 2560
#define WORLD_OFFSET_Z 2560
#define WORLD_SPACING 20

#define CMD_IDLE           0
#define CMD_BOOT_WINDOW    1
#define CMD_KILL_WINDOW    2
#define CMD_REBUILD_WSI    3
#define CMD_PREPARE_NEW_WSI 4
#define CMD_FLIP_WSI 5
#define CMD_TEARDOWN_WSI 6
#define MAX_WINDOWS        4
#define RING_SIZE          16
#define TRANSFER_RING_SIZE 16

/* ── Engine Memory Structures ────────────────────────────────────────────── */
typedef struct  {
    VkDevice device;
    VkQueue queue;
    VkQueue transfer_queue;
    uint32_t max_frames_in_flight;
    uint8_t _pad_auto_0[4];
    void* vkWaitForFences;
    void* vkAcquireNextImageKHR;
    void* vkResetFences;
    void* vkQueueSubmit;
    void* vkQueuePresentKHR;
    void* pfnBegin;
    void* pfnEnd;
    void* pfnSetCullMode;
    void* pfnSetFrontFace;
    void* pfnSetPrimitiveTopology;
    void* pfnSetDepthTestEnable;
    void* pfnSetDepthWriteEnable;
    void* pfnSetDepthCompareOp;
} VulkanDeviceContext;

typedef struct  {
    VkSwapchainKHR swapchain;
    uint32_t status;
    uint8_t _pad_auto_0[4];
    uint64_t swapchain_images[10];
    uint64_t swapchain_views[10];
    VkSemaphore image_available[10];
    VkSemaphore render_finished[10];
    VkFence in_flight[10];
    VkFence images_in_flight_fences[10];
} VulkanSwapchainContext;

typedef struct  {
    float m[16];
} mat4_t;

typedef struct  {
    float px;
    float py;
    float pz;
    uint32_t tile_data;
} RtsTileInstance;

typedef struct  {
    mat4_t viewProj;
    uint32_t aos_current_idx;
    uint32_t aos_prev_idx;
    float dt;
    float total_time;
    uint32_t target_state;
    uint32_t hover_idx;
    uint32_t flags;
    uint8_t _pad_tail[4];
} PushConstants;

typedef struct  {
    uint64_t pipeline_id;
    uint64_t descriptor_set;
    uint32_t index_count;
    uint32_t instance_count;
    uint32_t first_index;
    int32_t vertex_offset;
    uint32_t first_instance;
    uint16_t pc_offset;
    uint16_t pc_size;
    uint8_t push_constants[128];
    int16_t scissor_x;
    int16_t scissor_y;
    uint16_t scissor_w;
    uint16_t scissor_h;
    uint8_t cull_mode;
    uint8_t depth_test;
    uint8_t depth_write;
    uint8_t depth_compare_op;
    uint8_t front_face;
    uint8_t topology;
    uint8_t _pad_tail[2];
} DrawCommand;

typedef struct __attribute__((aligned(64))) {
    DrawCommand* draw_queue;
    uint32_t draw_count;
    uint32_t target_window_id;
    uint64_t gfx_layout;
    uint64_t vertex_buffer;
    uint64_t index_buffer;
    uint64_t depth_image;
    uint64_t depth_view;
    uint32_t width;
    uint32_t height;
    uint32_t swapchain_generation;
    uint8_t _pad_tail[60];
} RenderPacket;

#pragma pack(push, 1)
typedef struct {
    uint8_t opcode;
    uint8_t flags;
    uint16_t target_id;
    uint32_t target_pos;
} PlayerCommand;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint64_t session_token;
    uint32_t frame_tick;
    uint32_t checksum_tick;
    uint32_t state_checksum;
    uint32_t base_tick;
    uint8_t player_id;
    uint8_t history_count;
    uint16_t _align_pad;
    uint32_t peer_acks[8];
    PlayerCommand commands[120][2];
} LockstepPacket;
#pragma pack(pop)

typedef struct __attribute__((aligned(4))) {
    uint32_t tick;
    uint8_t state;
    uint8_t _pad_auto_0[3];
    uint32_t state_checksum;
    uint32_t remote_checksum;
    uint8_t remote_peer_id;
    uint8_t _pad_auto_1[7];
    PlayerCommand commands[8][2];
} NetworkFrame;

typedef struct __attribute__((aligned(64))) {
    uint32_t head_tick;
    uint32_t confirmed_tick;
    uint8_t is_rollback_active;
    uint8_t _pad_auto_0[3];
    uint32_t rollback_target;
    uint8_t _pad_auto_1[136];
    NetworkFrame frames[512];
    uint8_t _pad_tail[40];
} RollbackBuffer;

typedef struct  {
    uint16_t len;
    uint8_t data[4096];
} RxPacket;

#pragma pack(push, 1)
typedef struct {
    uint64_t session_token;
    uint32_t frame_tick;
    uint32_t checksum_tick;
    uint32_t state_checksum;
    uint32_t base_tick;
    uint8_t player_id;
    uint8_t is_ping;
    uint16_t _align_pad;
    uint32_t padding[3];
} IcePunchPacket;
#pragma pack(pop)

/* ── Core Data Types ─────────────────────────────────────────────────────── */
typedef struct {
    _Atomic(void*)  vk_instance;
    _Atomic(void*)  vk_surface;
    _Atomic int     glfw_cmd;
    _Atomic int     glfw_arg_w;
    _Atomic int     glfw_arg_h;
    _Atomic int     last_key_pressed;
    _Atomic uint32_t wasd_mask;
    _Atomic float   mouse_dx;
    _Atomic float   mouse_dy;
    _Atomic float   mouse_x;
    _Atomic float   mouse_y;
    _Atomic int     mouse_captured;
    _Atomic int     window_resized;
    _Atomic int     win_w;
    _Atomic int     win_h;
    _Atomic float   click_x;
    _Atomic float   click_y;
    _Atomic int     mouse_left;
    _Atomic int     mouse_right;
    _Atomic int     key_space;
    uint8_t         _pad[40];
} TenantMailbox;

_Static_assert(sizeof(TenantMailbox) == 128,
               "TenantMailbox must prevent false sharing");

typedef struct {
    alignas(64) _Atomic int ready_index;
    _Atomic int is_running;
    _Atomic int lua_finished;
    _Atomic int active_window;
    alignas(64) TenantMailbox tenants[MAX_WINDOWS];
} IPC_Mailbox;

typedef struct {
    IPC_Mailbox mailbox;
    int render_index;
    int write_index;
} EngineState;

typedef struct {
    alignas(64) RenderPacket packets[RING_SIZE];
    alignas(64) _Atomic int  ready_idx[MAX_WINDOWS];
    alignas(64) _Atomic int  local_read[MAX_WINDOWS];
    alignas(64) int          active_ring_slots[MAX_WINDOWS][10];
    alignas(64) _Atomic uint32_t locked_mask;
} RenderRing;

typedef struct {
    uint64_t src_buffer;
    uint64_t dst_buffer;
    uint64_t size;
    uint64_t timeline_sem;
    uint64_t signal_val;
    int      target_window_id;
    uint32_t _pad;
    alignas(64) _Atomic int status;
} TransferJob;

/* ── Extern Global State ─────────────────────────────────────────────────── */
#ifdef __cplusplus
extern "C" {
#endif

extern EngineState       g_engine;
extern RenderRing        g_ring;

extern VulkanDeviceContext    g_device_ctx[MAX_WINDOWS];
extern VulkanSwapchainContext g_wsi_ctx[MAX_WINDOWS][2];
extern _Atomic uint32_t       g_wsi_generation[MAX_WINDOWS];

extern atomic_int        g_wsi_state[MAX_WINDOWS];
extern atomic_int g_render_busy[MAX_WINDOWS];
extern atomic_int g_transfer_busy[MAX_WINDOWS];

extern vmath_thread_t    g_render_thread;
extern atomic_int        g_render_thread_active;

extern TransferJob       g_transfer_ring[TRANSFER_RING_SIZE];

extern vmath_thread_t    g_transfer_thread;
extern atomic_int        g_transfer_thread_active;

extern VkCommandPool     g_render_cmd_pools[MAX_WINDOWS];
extern VkCommandPool     g_transfer_cmd_pools[MAX_WINDOWS];
extern VkCommandBuffer   g_render_cmd_buffers[MAX_WINDOWS][3];
extern VkCommandBuffer   g_transfer_cmd_buffers[MAX_WINDOWS];
extern VkFence           g_render_fences[MAX_WINDOWS][3];
extern VkFence           g_transfer_fences[MAX_WINDOWS];

#ifdef __cplusplus
}
#endif
