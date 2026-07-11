<render_thread_loop>:
lea    r10,[rsp+0x8]
and    rsp,0xffffffffffffffe0
push   QWORD PTR [r10-0x8]
push   rbp
mov    rbp,rsp
push   r15
push   r14
push   r13
push   r12
push   r10
push   rbx
sub    rsp,0x160
mov    rdi,QWORD PTR fs:0x28
mov    QWORD PTR [rbp-0x38],rdi
lea    rdi,[rip+0x1499]        # <_IO_stdin_used+0x480>
call <puts@plt>
vpxor  xmm0,xmm0,xmm0
vmovdqa XMMWORD PTR [rbp-0x50],xmm0
mov    eax,DWORD PTR [rip+0x4c05]        # <g_render_thread_active>
test   eax,eax
je <render_thread_loop+0x687>
xchg   ax,ax
data16 cs nop WORD PTR [rax+rax*1+0x0]
mov    eax,DWORD PTR [rip+0x676e]        # <g_engine+0x4>
test   eax,eax
je <render_thread_loop+0x687>
lea    r11,[rip+0x5bbb]        # <g_device_ctx>
lea    r15,[rip+0x67a4]        # <g_engine+0x50>
xor    r12d,r12d
mov    QWORD PTR [rbp-0x110],r11
mov    r14,r11
mov    eax,DWORD PTR [r15]
cmp    eax,0x3
je <render_thread_loop+0x6d0>
add    r12,0x4
sub    r15,0xffffffffffffff80
add    r14,0x88
cmp    r12,0x10
jne <render_thread_loop+0x89>
lea    rax,[rip+0x6797]        # <g_engine+0x78>
mov    r11,QWORD PTR [rbp-0x110]
lea    r15,[rip+0x6591]        # <g_ring+0x800>
xor    ebx,ebx
mov    QWORD PTR [rbp-0x138],rax
mov    QWORD PTR [rbp-0x108],0x0
mov    r13,r11
xor    r11d,r11d
mov    eax,DWORD PTR [r15]
mov    edx,DWORD PTR [r15+0x40]
cmp    eax,0xffffffff
je <render_thread_loop+0x642>
cmp    eax,edx
je <render_thread_loop+0x642>
mov    eax,edx
mov    r12d,r11d
mov    r8d,0x77359400
sub    eax,ebx
lea    ecx,[rax+0x1]
mov    eax,ecx
sar    eax,0x1f
shr    eax,0x1e
lea    r9d,[rcx+rax*1]
and    r9d,0x3
sub    r9d,eax
lea    rax,[rip+0x4b55]        # <g_render_busy>
add    r9d,ebx
cmp    edx,0xffffffff
lea    rcx,[rbx+rax*1]
cmove  r9d,ebx
mov    DWORD PTR [r15+0x40],r9d
mov    QWORD PTR [rbp-0x120],rcx
lea    rcx,[rip+0x5d15]        # <g_ring>
mov    DWORD PTR [rax+rbx*1],0x1
movsxd rax,r9d
mov    DWORD PTR [rbp-0x130],r9d
mov    QWORD PTR [rbp-0x170],rax
shl    rax,0x7
mov    edx,DWORD PTR [r13+0x18]
mov    r14d,DWORD PTR [rcx+rax*1+0x40]
mov    eax,0x3
mov    DWORD PTR [rbp-0x128],r11d
mov    ecx,0x1
cmp    edx,eax
cmovbe eax,edx
test   edx,edx
mov    edx,0x3
cmovne edx,eax
mov    eax,DWORD PTR [rbp+rbx*1-0x50]
mov    esi,edx
mov    DWORD PTR [rbp-0x114],edx
xor    edx,edx
div    esi
lea    rax,[r12+r12*2]
mov    esi,0x1
mov    edi,edx
mov    DWORD PTR [rbp-0x118],edx
lea    rdx,[rip+0x4148]        # <g_render_fences>
add    rax,rdi
mov    QWORD PTR [rbp-0x110],rdi
mov    rdi,QWORD PTR [r13+0x0]
mov    rax,QWORD PTR [rdx+rax*8]
lea    rdx,[rbp-0xf8]
mov    QWORD PTR [rbp-0xf8],rax
mov    rax,QWORD PTR [r13+0x20]
mov    QWORD PTR [rbp-0x140],rax
call   rax
mov    rcx,QWORD PTR [rbp-0x108]
mov    r11d,DWORD PTR [rbp-0x128]
cmp    eax,0x2
mov    r9d,DWORD PTR [rbp-0x130]
lea    r10,[rcx*8+0x0]
je <render_thread_loop+0x7d0>
mov    rcx,QWORD PTR [rbp-0x110]
lea    rax,[r12+r12*4]
lea    rsi,[rip+0x5c43]        # <g_ring>
lea    rax,[rcx+rax*2+0x220]
mov    ecx,DWORD PTR [rsi+rax*4]
cmp    ecx,0xffffffff
je <render_thread_loop+0x230>
cmp    r9d,ecx
je <render_thread_loop+0x230>
mov    eax,0xfffffffe
rol    eax,cl
lock and DWORD PTR [rip+0x6560],eax        # <g_ring+0x940>
mov    rdi,QWORD PTR [rbp-0x110]
lea    rax,[r12+r12*4]
and    r14d,0x1
lea    rsi,[rip+0x5c0a]        # <g_ring>
lea    rcx,[rip+0x4a6b]        # <g_wsi_ctx+0x8>
lea    r8,[rip+0x4a5c]        # <g_wsi_ctx>
imul   rdx,r12,0x3e0
lea    rax,[rdi+rax*2+0x220]
mov    DWORD PTR [rsi+rax*4],r9d
mov    rax,r14
neg    rax
mov    QWORD PTR [rbp-0x148],rax
and    eax,0x1f0
add    rax,r10
mov    QWORD PTR [rbp-0x130],rax
mov    ecx,DWORD PTR [rcx+rax*1]
lea    rax,[rip+0x4a23]        # <g_wsi_ctx>
mov    QWORD PTR [rbp-0x128],rax
imul   rax,r14,0x1f0
add    rax,rdx
add    rax,r8
cmp    QWORD PTR [rax],0x0
je <render_thread_loop+0x5f0>
lea    rdx,[rip+0x49ce]        # <g_wsi_state>
mov    edx,DWORD PTR [rdx+rbx*1]
test   edx,edx
je <render_thread_loop+0x5f0>
mov    rdx,QWORD PTR [rbp-0x170]
shl    rdx,0x7
add    rsi,rdx
mov    r9d,DWORD PTR [rsi+0x38]
mov    QWORD PTR [rbp-0x150],rsi
test   r9d,r9d
je <render_thread_loop+0x5f0>
mov    esi,DWORD PTR [rsi+0x3c]
test   esi,esi
je <render_thread_loop+0x5f0>
cmp    ecx,0x1
jne <render_thread_loop+0x5f0>
lea    rdx,[r12+r12*2]
lea    rcx,[rip+0x4072]        # <g_render_cmd_buffers>
mov    rsi,QWORD PTR [rax]
mov    DWORD PTR [rbp-0x17c],r11d
add    rdx,rdi
mov    QWORD PTR [rbp-0x178],r10
lea    r9,[rbp-0x100]
mov    rdx,QWORD PTR [rcx+rdx*8]
imul   rcx,r14,0x3e
mov    QWORD PTR [rbp-0xf0],rdx
imul   rdx,r12,0x7c
add    rcx,rdx
lea    rdx,[rcx+rdi*1+0x14]
mov    QWORD PTR [rbp-0x188],rcx
mov    rdi,QWORD PTR [r13+0x0]
mov    rcx,QWORD PTR [r8+rdx*8+0x10]
xor    r8d,r8d
mov    edx,0x4c4b40
call   QWORD PTR [r13+0x28]
mov    r10,QWORD PTR [rbp-0x178]
mov    r11d,DWORD PTR [rbp-0x17c]
lea    edx,[rax-0x1]
cmp    edx,0x1
jbe <render_thread_loop+0x5f0>
cmp    eax,0xc4653214
je <render_thread_loop+0x810>
mov    rcx,QWORD PTR [rbp-0x148]
mov    rdi,QWORD PTR [rbp-0x188]
mov    eax,DWORD PTR [rbp-0x100]
and    ecx,0x3e
lea    rdx,[rdi+rax*1+0x34]
lea    rdi,[rip+0x4918]        # <g_wsi_ctx>
mov    QWORD PTR [rbp-0x148],rcx
cmp    QWORD PTR [rdi+rdx*8],0x0
je <render_thread_loop+0x3f5>
mov    rsi,QWORD PTR [rbp-0x108]
mov    DWORD PTR [rbp-0x17c],r11d
mov    r8d,0x77359400
mov    QWORD PTR [rbp-0x178],r10
lea    rax,[rsi+rax*1+0x34]
mov    esi,0x1
add    rax,rcx
mov    ecx,0x1
lea    rdx,[rdi+rax*8]
mov    rdi,QWORD PTR [r13+0x0]
call   QWORD PTR [rbp-0x140]
mov    eax,DWORD PTR [rbp-0x100]
mov    r11d,DWORD PTR [rbp-0x17c]
mov    r10,QWORD PTR [rbp-0x178]
imul   r14,r14,0x3e
mov    rdi,QWORD PTR [r13+0x0]
mov    esi,0x1
mov    rdx,QWORD PTR [rbp-0xf8]
imul   r12,r12,0x7c
mov    DWORD PTR [rbp-0x17c],r11d
mov    QWORD PTR [rbp-0x178],r10
add    r12,r14
lea    r14,[rip+0x488b]        # <g_wsi_ctx>
lea    rax,[r12+rax*1+0x34]
mov    QWORD PTR [r14+rax*8],rdx
lea    rdx,[rbp-0xf8]
call   QWORD PTR [r13+0x30]
mov    eax,DWORD PTR [rbp-0x100]
mov    rdi,QWORD PTR [rbp-0xf0]
xor    esi,esi
add    rax,r12
mov    r9,QWORD PTR [r14+rax*8+0x10]
mov    r14,QWORD PTR [r14+rax*8+0x60]
mov    QWORD PTR [rbp-0x140],r9
call <vkResetCommandBuffer@plt>
mov    rax,QWORD PTR [rbp-0x170]
sub    rsp,0x8
lea    rdi,[rip+0x59dd]        # <g_ring>
mov    r9,QWORD PTR [rbp-0x140]
mov    rsi,QWORD PTR [rbp-0x150]
mov    r8,r13
shl    rax,0x7
add    rax,rdi
mov    rdi,QWORD PTR [rbp-0xf0]
mov    ecx,DWORD PTR [rax+0x8]
mov    rdx,QWORD PTR [rax]
push   r14
call <vx_record_commands>
mov    eax,DWORD PTR [rbp-0x100]
vpxor  xmm0,xmm0,xmm0
mov    DWORD PTR [rbp-0xfc],0x400
pop    rdx
pop    rcx
lea    rcx,[rip+0x47f4]        # <g_wsi_ctx>
mov    esi,0x1
lea    rax,[r12+rax*1+0x20]
mov    r14,rcx
mov    rdi,QWORD PTR [rbp-0x148]
vmovdqu YMMWORD PTR [rbp-0x9c],ymm0
mov    rax,QWORD PTR [rcx+rax*8]
vmovdqu YMMWORD PTR [rbp-0x80],ymm0
lea    r12,[rbp-0xe8]
lea    rdx,[rbp-0xa0]
mov    DWORD PTR [rbp-0xa0],0x4
mov    QWORD PTR [rbp-0xe8],rax
mov    rax,QWORD PTR [rbp-0x108]
mov    DWORD PTR [rbp-0x90],0x1
lea    rax,[rax+rdi*1+0x16]
add    rax,QWORD PTR [rbp-0x110]
mov    DWORD PTR [rbp-0x78],0x1
lea    rax,[rcx+rax*8]
mov    QWORD PTR [rbp-0x60],r12
mov    rdi,QWORD PTR [r13+0x8]
mov    QWORD PTR [rbp-0x88],rax
lea    rax,[rbp-0xfc]
mov    rcx,QWORD PTR [rbp-0xf8]
mov    QWORD PTR [rbp-0x80],rax
lea    rax,[rbp-0xf0]
mov    QWORD PTR [rbp-0x70],rax
mov    DWORD PTR [rbp-0x68],0x1
vmovdqa YMMWORD PTR [rbp-0x170],ymm0
vzeroupper
call   QWORD PTR [r13+0x38]
mov    rax,QWORD PTR [rbp-0x130]
vmovdqa ymm0,YMMWORD PTR [rbp-0x170]
mov    DWORD PTR [rbp-0xe0],0x3b9acde9
mov    rdi,QWORD PTR [r13+0x8]
lea    rsi,[rbp-0xe0]
add    rax,r14
vmovdqu YMMWORD PTR [rbp-0xdc],ymm0
vmovdqu YMMWORD PTR [rbp-0xc0],ymm0
mov    QWORD PTR [rbp-0xb8],rax
lea    rax,[rbp-0x100]
mov    QWORD PTR [rbp-0xb0],rax
mov    DWORD PTR [rbp-0xd0],0x1
mov    QWORD PTR [rbp-0xc8],r12
mov    DWORD PTR [rbp-0xc0],0x1
vzeroupper
call   QWORD PTR [r13+0x40]
mov    r11d,DWORD PTR [rbp-0x17c]
mov    r10,QWORD PTR [rbp-0x178]
cs nop WORD PTR [rax+rax*1+0x0]
lea    rax,[rip+0x4699]        # <g_wsi_generation>
mov    eax,DWORD PTR [rax+rbx*1]
add    eax,0x1
and    eax,0x1
mov    rsi,QWORD PTR [rbp-0x128]
neg    rax
and    eax,0x1f0
lea    rax,[r10+rax*1+0x8]
add    rax,rsi
cmp    DWORD PTR [rax],0x2
ja <render_thread_loop+0x6c0>
xor    edx,edx
mov    rax,QWORD PTR [rbp-0x120]
mov    DWORD PTR [rax],0x0
mov    eax,DWORD PTR [rbp-0x118]
add    eax,0x1
div    DWORD PTR [rbp-0x114]
mov    DWORD PTR [rbp+rbx*1-0x50],edx
add    r11d,0x1
add    QWORD PTR [rbp-0x108],0x7c
add    r15,0x4
add    r13,0x88
sub    QWORD PTR [rbp-0x138],0xffffffffffffff80
add    rbx,0x4
cmp    r11d,0x4
jne <render_thread_loop+0xd9>
mov    edi,0x3e8
call <usleep@plt>
mov    eax,DWORD PTR [rip+0x45d1]        # <g_render_thread_active>
test   eax,eax
jne <render_thread_loop+0x60>
lea    rdi,[rip+0xeb2]        # <_IO_stdin_used+0x4f0>
call <puts@plt>
mov    rax,QWORD PTR [rbp-0x38]
sub    rax,QWORD PTR fs:0x28
jne <render_thread_loop+0x848>
lea    rsp,[rbp-0x30]
xor    eax,eax
pop    rbx
pop    r10
pop    r12
pop    r13
pop    r14
pop    r15
pop    rbp
lea    rsp,[r10-0x8]
ret
nop    DWORD PTR [rax]
lock sub DWORD PTR [rax],0x1
jmp <render_thread_loop+0x620>
nop    DWORD PTR [rax+0x0]
lea    rax,[rip+0x4589]        # <g_transfer_busy>
xor    ebx,ebx
mov    r13d,0x7d0
add    rax,r12
mov    QWORD PTR [rbp-0x108],rax
jmp <render_thread_loop+0x701>
nop    DWORD PTR [rax+rax*1+0x0]
mov    edi,0x3e8
sub    r13d,0x1
add    ebx,0x1
call <usleep@plt>
mov    rax,QWORD PTR [rbp-0x108]
mov    eax,DWORD PTR [rax]
test   eax,eax
je <render_thread_loop+0x750>
test   r13d,r13d
je <render_thread_loop+0x750>
cmp    ebx,0x7cf
jg <render_thread_loop+0x6f0>
cmp    ebx,0x3e7
jg <render_thread_loop+0x737>
pause
mov    rax,QWORD PTR [rbp-0x108]
mov    eax,DWORD PTR [rax]
test   eax,eax
je <render_thread_loop+0x750>
add    ebx,0x1
jmp <render_thread_loop+0x713>
call <sched_yield@plt>
mov    rax,QWORD PTR [rbp-0x108]
mov    eax,DWORD PTR [rax]
test   eax,eax
jne <render_thread_loop+0x732>
nop    DWORD PTR [rax+0x0]
cmp    QWORD PTR [r14],0x0
je <render_thread_loop+0x7a6>
mov    rdi,QWORD PTR [r14+0x8]
test   rdi,rdi
je <render_thread_loop+0x764>
call <vkQueueWaitIdle@plt>
mov    rdi,QWORD PTR [r14+0x10]
test   rdi,rdi
je <render_thread_loop+0x772>
call <vkQueueWaitIdle@plt>
lea    rax,[rip+0x3c77]        # <g_render_cmd_pools>
mov    rsi,QWORD PTR [rax+r12*2]
test   rsi,rsi
je <render_thread_loop+0x78c>
mov    rdi,QWORD PTR [r14]
xor    edx,edx
call <vkResetCommandPool@plt>
lea    rax,[rip+0x3c3d]        # <g_transfer_cmd_pools>
mov    rsi,QWORD PTR [rax+r12*2]
test   rsi,rsi
je <render_thread_loop+0x7a6>
mov    rdi,QWORD PTR [r14]
xor    edx,edx
call <vkResetCommandPool@plt>
lea    rax,[rip+0x44d3]        # <g_wsi_state>
mov    DWORD PTR [rax+r12*1],0x0
mov    DWORD PTR [r15+0x28],0x0
mov    DWORD PTR [r15],0x0
jmp <render_thread_loop+0x95>
nop    DWORD PTR [rax+0x0]
mov    esi,r11d
lea    rdi,[rip+0xd26]        # <_IO_stdin_used+0x4b0>
xor    eax,eax
mov    QWORD PTR [rbp-0x130],r10
mov    DWORD PTR [rbp-0x110],r11d
call <printf@plt>
lea    rax,[rip+0x44ba]        # <g_wsi_ctx>
mov    r11d,DWORD PTR [rbp-0x110]
mov    r10,QWORD PTR [rbp-0x130]
mov    QWORD PTR [rbp-0x128],rax
jmp <render_thread_loop+0x5f0>
mov    DWORD PTR [rbp-0x130],r11d
mov    edi,0x2710
mov    QWORD PTR [rbp-0x110],r10
mov    rax,QWORD PTR [rbp-0x138]
mov    DWORD PTR [rax],0x1
call <usleep@plt>
mov    r10,QWORD PTR [rbp-0x110]
mov    r11d,DWORD PTR [rbp-0x130]
jmp <render_thread_loop+0x5f0>
call <__stack_chk_fail@plt>
nop    DWORD PTR [rax]

