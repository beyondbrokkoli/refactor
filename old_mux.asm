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
lea    rdi,[rip+0x1a39]        # <_IO_stdin_used+0x390>
call <puts@plt>
vpxor  xmm0,xmm0,xmm0
vmovdqa XMMWORD PTR [rbp-0x50],xmm0
mov    eax,DWORD PTR [rip+0x52d5]        # <g_render_thread_active>
test   eax,eax
je <render_thread_loop+0x6a7>
xchg   ax,ax
data16 cs nop WORD PTR [rax+rax*1+0x0]
mov    eax,DWORD PTR [rip+0x6e3e]        # <g_engine+0x4>
test   eax,eax
je <render_thread_loop+0x6a7>
lea    r11,[rip+0x628b]        # <g_device_ctx>
lea    r15,[rip+0x6e74]        # <g_engine+0x50>
xor    r12d,r12d
mov    QWORD PTR [rbp-0x110],r11
mov    r14,r11
mov    eax,DWORD PTR [r15]
cmp    eax,0x3
je <render_thread_loop+0x6f0>
add    r12,0x4
sub    r15,0xffffffffffffff80
add    r14,0x88
cmp    r12,0x10
jne <render_thread_loop+0x89>
lea    rax,[rip+0x6e67]        # <g_engine+0x78>
mov    r14,QWORD PTR [rbp-0x110]
xor    ebx,ebx
xor    r10d,r10d
mov    QWORD PTR [rbp-0x158],rax
lea    r15,[rip+0x6c55]        # <g_ring+0x800>
mov    QWORD PTR [rbp-0x108],0x0
mov    eax,DWORD PTR [r15]
mov    edx,DWORD PTR [r15+0x40]
cmp    eax,0xffffffff
je <render_thread_loop+0x662>
cmp    eax,edx
je <render_thread_loop+0x662>
mov    eax,edx
mov    r12d,r10d
mov    esi,0x1
mov    r8d,0x77359400
sub    eax,ebx
lea    ecx,[rax+0x1]
mov    eax,ecx
sar    eax,0x1f
shr    eax,0x1e
lea    r9d,[rcx+rax*1]
and    r9d,0x3
sub    r9d,eax
lea    rax,[rip+0x5223]        # <g_render_busy>
add    r9d,ebx
cmp    edx,0xffffffff
lea    rcx,[rbx+rax*1]
cmove  r9d,ebx
mov    DWORD PTR [r15+0x40],r9d
mov    QWORD PTR [rbp-0x118],rcx
lea    rcx,[rip+0x63e3]        # <g_ring>
mov    DWORD PTR [rax+rbx*1],0x1
movsxd rax,r9d
mov    DWORD PTR [rbp-0x150],r9d
mov    QWORD PTR [rbp-0x160],rax
shl    rax,0x7
mov    eax,DWORD PTR [rcx+rax*1+0x40]
mov    edx,DWORD PTR [r14+0x18]
mov    DWORD PTR [rbp-0x128],r10d
mov    ecx,0x1
mov    DWORD PTR [rbp-0x130],eax
mov    eax,0x3
cmp    edx,eax
cmovbe eax,edx
test   edx,edx
mov    edx,0x3
cmovne edx,eax
mov    eax,DWORD PTR [rbp+rbx*1-0x50]
mov    edi,edx
mov    DWORD PTR [rbp-0x110],edx
xor    edx,edx
div    edi
mov    rdi,QWORD PTR [r14]
lea    rax,[r12+r12*2]
mov    r13d,edx
mov    DWORD PTR [rbp-0x11c],edx
lea    rdx,[rip+0x4812]        # <g_render_fences>
add    rax,r13
mov    rax,QWORD PTR [rdx+rax*8]
lea    rdx,[rbp-0xf8]
mov    QWORD PTR [rbp-0xf8],rax
mov    rax,QWORD PTR [r14+0x20]
mov    QWORD PTR [rbp-0x168],rax
call   rax
mov    rsi,QWORD PTR [rbp-0x108]
mov    r10d,DWORD PTR [rbp-0x128]
cmp    eax,0x2
mov    r9d,DWORD PTR [rbp-0x150]
lea    r11,[rsi*8+0x0]
je <render_thread_loop+0x7f0>
lea    rax,[r12+r12*4]
lea    rdi,[rip+0x631f]        # <g_ring>
lea    rax,[r13+rax*2+0x220]
mov    ecx,DWORD PTR [rdi+rax*4]
cmp    ecx,0xffffffff
je <render_thread_loop+0x224>
cmp    r9d,ecx
je <render_thread_loop+0x224>
mov    eax,0xfffffffe
rol    eax,cl
lock and DWORD PTR [rip+0x6c3c],eax        # <g_ring+0x940>
mov    esi,DWORD PTR [rbp-0x130]
lea    rax,[r12+r12*4]
lea    rdi,[rip+0x62eb]        # <g_ring>
lea    rax,[r13+rax*2+0x220]
lea    rcx,[rip+0x5144]        # <g_wsi_ctx+0x8>
and    esi,0x1
mov    DWORD PTR [rdi+rax*4],r9d
imul   rdx,r12,0x3e0
lea    r8,[rip+0x5127]        # <g_wsi_ctx>
mov    rax,rsi
mov    QWORD PTR [rbp-0x150],rsi
neg    rax
mov    QWORD PTR [rbp-0x170],rax
and    eax,0x1f0
add    rax,r11
mov    QWORD PTR [rbp-0x130],rax
mov    ecx,DWORD PTR [rcx+rax*1]
lea    rax,[rip+0x50fa]        # <g_wsi_ctx>
mov    QWORD PTR [rbp-0x128],rax
imul   rax,rsi,0x1f0
add    rax,rdx
add    rax,r8
cmp    QWORD PTR [rax],0x0
je <render_thread_loop+0x610>
lea    rdx,[rip+0x50a5]        # <g_wsi_state>
mov    edx,DWORD PTR [rdx+rbx*1]
test   edx,edx
je <render_thread_loop+0x610>
mov    rdx,QWORD PTR [rbp-0x160]
shl    rdx,0x7
add    rdx,rdi
mov    edi,DWORD PTR [rdx+0x38]
test   edi,edi
je <render_thread_loop+0x610>
mov    r9d,DWORD PTR [rdx+0x3c]
test   r9d,r9d
je <render_thread_loop+0x610>
cmp    ecx,0x1
jne <render_thread_loop+0x610>
lea    rdx,[r12+r12*2]
lea    rcx,[rip+0x4750]        # <g_render_cmd_buffers>
mov    rdi,QWORD PTR [r14]
mov    DWORD PTR [rbp-0x120],r10d
add    rdx,r13
mov    QWORD PTR [rbp-0x178],r11
lea    r9,[rbp-0x100]
mov    rdx,QWORD PTR [rcx+rdx*8]
imul   rcx,rsi,0x3e
mov    QWORD PTR [rbp-0xf0],rdx
imul   rdx,r12,0x7c
lea    rsi,[rcx+rdx*1]
lea    rdx,[rsi+r13*1+0x14]
mov    QWORD PTR [rbp-0x180],rsi
mov    rsi,QWORD PTR [rax]
mov    rcx,QWORD PTR [r8+rdx*8+0x10]
xor    r8d,r8d
mov    edx,0x4c4b40
call   QWORD PTR [r14+0x28]
mov    r11,QWORD PTR [rbp-0x178]
mov    r10d,DWORD PTR [rbp-0x120]
lea    edx,[rax-0x1]
cmp    edx,0x1
jbe <render_thread_loop+0x610>
cmp    eax,0xc4653214
je <render_thread_loop+0x879>
mov    rsi,QWORD PTR [rbp-0x170]
mov    rdi,QWORD PTR [rbp-0x180]
mov    eax,DWORD PTR [rbp-0x100]
and    esi,0x3e
lea    rdx,[rdi+rax*1+0x34]
lea    rdi,[rip+0x4ff6]        # <g_wsi_ctx>
mov    QWORD PTR [rbp-0x170],rsi
cmp    QWORD PTR [rdi+rdx*8],0x0
je <render_thread_loop+0x3e6>
mov    rcx,QWORD PTR [rbp-0x108]
mov    DWORD PTR [rbp-0x120],r10d
mov    r8d,0x77359400
mov    QWORD PTR [rbp-0x178],r11
lea    rax,[rcx+rax*1+0x34]
mov    ecx,0x1
add    rax,rsi
mov    esi,0x1
lea    rdx,[rdi+rax*8]
mov    rdi,QWORD PTR [r14]
call   QWORD PTR [rbp-0x168]
mov    eax,DWORD PTR [rbp-0x100]
mov    r10d,DWORD PTR [rbp-0x120]
mov    r11,QWORD PTR [rbp-0x178]
imul   rdx,r12,0x7c
lea    rsi,[rip+0x4f8f]        # <g_wsi_ctx>
mov    rdi,QWORD PTR [r14]
mov    DWORD PTR [rbp-0x120],r10d
imul   rcx,QWORD PTR [rbp-0x150],0x3e
mov    QWORD PTR [rbp-0x178],r11
add    rcx,rdx
mov    rdx,QWORD PTR [rbp-0xf8]
lea    rax,[rcx+rax*1+0x34]
mov    QWORD PTR [rbp-0x168],rcx
mov    QWORD PTR [rsi+rax*8],rdx
lea    rdx,[rbp-0xf8]
mov    esi,0x1
call   QWORD PTR [r14+0x30]
lea    rsi,[rip+0x4f45]        # <g_wsi_ctx>
mov    eax,DWORD PTR [rbp-0x100]
add    rax,QWORD PTR [rbp-0x168]
mov    rdi,QWORD PTR [rsi+rax*8+0x10]
mov    rax,QWORD PTR [rsi+rax*8+0x60]
xor    esi,esi
mov    QWORD PTR [rbp-0x168],rdi
mov    rdi,QWORD PTR [rbp-0xf0]
mov    QWORD PTR [rbp-0x180],rax
call <vkResetCommandBuffer@plt>
mov    rsi,QWORD PTR [rbp-0x160]
lea    rax,[rip+0x60a4]        # <g_ring>
mov    r11,QWORD PTR [rbp-0x178]
mov    r10d,DWORD PTR [rbp-0x120]
shl    rsi,0x7
add    rsi,rax
mov    edi,DWORD PTR [rsi+0x38]
test   edi,edi
je <render_thread_loop+0x4a3>
mov    ecx,DWORD PTR [rsi+0x3c]
test   ecx,ecx
jne <render_thread_loop+0x830>
imul   r12,r12,0x7c
mov    edx,DWORD PTR [rbp-0x100]
vpxor  xmm0,xmm0,xmm0
lea    rdi,[rip+0x4ec8]        # <g_wsi_ctx>
imul   rax,QWORD PTR [rbp-0x150],0x3e
mov    rsi,QWORD PTR [rbp-0x170]
vmovdqu YMMWORD PTR [rbp-0x9c],ymm0
vmovdqu YMMWORD PTR [rbp-0x80],ymm0
mov    rcx,QWORD PTR [rbp-0xf8]
mov    DWORD PTR [rbp-0x168],r10d
add    rax,r12
lea    r12,[rbp-0xe8]
mov    QWORD PTR [rbp-0x160],r11
lea    rax,[rdx+rax*1+0x20]
mov    QWORD PTR [rbp-0x60],r12
lea    rdx,[rbp-0xa0]
mov    rax,QWORD PTR [rdi+rax*8]
mov    DWORD PTR [rbp-0xfc],0x400
mov    DWORD PTR [rbp-0xa0],0x4
mov    QWORD PTR [rbp-0xe8],rax
mov    rax,QWORD PTR [rbp-0x108]
mov    DWORD PTR [rbp-0x90],0x1
lea    rax,[rax+rsi*1+0x16]
mov    DWORD PTR [rbp-0x78],0x1
mov    esi,0x1
add    rax,r13
mov    DWORD PTR [rbp-0x68],0x1
mov    r13,rdi
lea    rax,[rdi+rax*8]
vmovdqa YMMWORD PTR [rbp-0x150],ymm0
mov    rdi,QWORD PTR [r14+0x8]
mov    QWORD PTR [rbp-0x88],rax
lea    rax,[rbp-0xfc]
mov    QWORD PTR [rbp-0x80],rax
lea    rax,[rbp-0xf0]
mov    QWORD PTR [rbp-0x70],rax
vzeroupper
call   QWORD PTR [r14+0x38]
mov    rax,QWORD PTR [rbp-0x130]
vmovdqa ymm0,YMMWORD PTR [rbp-0x150]
mov    DWORD PTR [rbp-0xe0],0x3b9acde9
mov    rdi,QWORD PTR [r14+0x8]
lea    rsi,[rbp-0xe0]
add    rax,r13
vmovdqu YMMWORD PTR [rbp-0xdc],ymm0
vmovdqu YMMWORD PTR [rbp-0xc0],ymm0
mov    QWORD PTR [rbp-0xb8],rax
lea    rax,[rbp-0x100]
mov    QWORD PTR [rbp-0xb0],rax
mov    DWORD PTR [rbp-0xd0],0x1
mov    QWORD PTR [rbp-0xc8],r12
mov    DWORD PTR [rbp-0xc0],0x1
vzeroupper
call   QWORD PTR [r14+0x40]
mov    r10d,DWORD PTR [rbp-0x168]
mov    r11,QWORD PTR [rbp-0x160]
nop    DWORD PTR [rax+0x0]
data16 cs nop WORD PTR [rax+rax*1+0x0]
lea    rax,[rip+0x4d49]        # <g_wsi_generation>
mov    eax,DWORD PTR [rax+rbx*1]
add    eax,0x1
and    eax,0x1
mov    rcx,QWORD PTR [rbp-0x128]
neg    rax
and    eax,0x1f0
lea    rax,[r11+rax*1+0x8]
add    rax,rcx
cmp    DWORD PTR [rax],0x2
ja <render_thread_loop+0x6e0>
xor    edx,edx
mov    rax,QWORD PTR [rbp-0x118]
mov    DWORD PTR [rax],0x0
mov    eax,DWORD PTR [rbp-0x11c]
add    eax,0x1
div    DWORD PTR [rbp-0x110]
mov    DWORD PTR [rbp+rbx*1-0x50],edx
add    r10d,0x1
add    QWORD PTR [rbp-0x108],0x7c
add    r15,0x4
add    r14,0x88
sub    QWORD PTR [rbp-0x158],0xffffffffffffff80
add    rbx,0x4
cmp    r10d,0x4
jne <render_thread_loop+0xd6>
mov    edi,0x3e8
call <usleep@plt>
mov    eax,DWORD PTR [rip+0x4c81]        # <g_render_thread_active>
test   eax,eax
jne <render_thread_loop+0x60>
lea    rdi,[rip+0x1432]        # <_IO_stdin_used+0x400>
call <puts@plt>
mov    rax,QWORD PTR [rbp-0x38]
sub    rax,QWORD PTR fs:0x28
jne <render_thread_loop+0x8b1>
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
jmp <render_thread_loop+0x640>
nop    DWORD PTR [rax+0x0]
lea    rax,[rip+0x4c39]        # <g_transfer_busy>
xor    ebx,ebx
mov    r13d,0x7d0
add    rax,r12
mov    QWORD PTR [rbp-0x108],rax
jmp <render_thread_loop+0x721>
nop    DWORD PTR [rax+rax*1+0x0]
mov    edi,0x3e8
sub    r13d,0x1
add    ebx,0x1
call <usleep@plt>
mov    rax,QWORD PTR [rbp-0x108]
mov    eax,DWORD PTR [rax]
test   eax,eax
je <render_thread_loop+0x770>
test   r13d,r13d
je <render_thread_loop+0x770>
cmp    ebx,0x7cf
jg <render_thread_loop+0x710>
cmp    ebx,0x3e7
jg <render_thread_loop+0x757>
pause
mov    rax,QWORD PTR [rbp-0x108]
mov    eax,DWORD PTR [rax]
test   eax,eax
je <render_thread_loop+0x770>
add    ebx,0x1
jmp <render_thread_loop+0x733>
call <sched_yield@plt>
mov    rax,QWORD PTR [rbp-0x108]
mov    eax,DWORD PTR [rax]
test   eax,eax
jne <render_thread_loop+0x752>
nop    DWORD PTR [rax+0x0]
cmp    QWORD PTR [r14],0x0
je <render_thread_loop+0x7c6>
mov    rdi,QWORD PTR [r14+0x8]
test   rdi,rdi
je <render_thread_loop+0x784>
call <vkQueueWaitIdle@plt>
mov    rdi,QWORD PTR [r14+0x10]
test   rdi,rdi
je <render_thread_loop+0x792>
call <vkQueueWaitIdle@plt>
lea    rax,[rip+0x4327]        # <g_render_cmd_pools>
mov    rsi,QWORD PTR [rax+r12*2]
test   rsi,rsi
je <render_thread_loop+0x7ac>
mov    rdi,QWORD PTR [r14]
xor    edx,edx
call <vkResetCommandPool@plt>
lea    rax,[rip+0x42ed]        # <g_transfer_cmd_pools>
mov    rsi,QWORD PTR [rax+r12*2]
test   rsi,rsi
je <render_thread_loop+0x7c6>
mov    rdi,QWORD PTR [r14]
xor    edx,edx
call <vkResetCommandPool@plt>
lea    rax,[rip+0x4b83]        # <g_wsi_state>
mov    DWORD PTR [rax+r12*1],0x0
mov    DWORD PTR [r15+0x28],0x0
mov    DWORD PTR [r15],0x0
jmp <render_thread_loop+0x95>
nop    DWORD PTR [rax+0x0]
mov    esi,r10d
lea    rdi,[rip+0x12a6]        # <_IO_stdin_used+0x3c0>
xor    eax,eax
mov    QWORD PTR [rbp-0x130],r11
mov    DWORD PTR [rbp-0x150],r10d
call <printf@plt>
lea    rax,[rip+0x4b6a]        # <g_wsi_ctx>
mov    r10d,DWORD PTR [rbp-0x150]
mov    r11,QWORD PTR [rbp-0x130]
mov    QWORD PTR [rbp-0x128],rax
jmp <render_thread_loop+0x610>
sub    rsp,0x8
mov    rdx,QWORD PTR [rsi]
mov    ecx,DWORD PTR [rsi+0x8]
mov    r8,r14
mov    DWORD PTR [rbp-0x178],r10d
mov    r9,QWORD PTR [rbp-0x168]
mov    QWORD PTR [rbp-0x160],r11
mov    rdi,QWORD PTR [rbp-0xf0]
push   QWORD PTR [rbp-0x180]
call <vx_record_commands.part.0>
pop    rax
pop    rdx
mov    r10d,DWORD PTR [rbp-0x178]
mov    r11,QWORD PTR [rbp-0x160]
jmp <render_thread_loop+0x4a3>
mov    DWORD PTR [rbp-0x130],r10d
mov    edi,0x2710
mov    QWORD PTR [rbp-0x150],r11
mov    rax,QWORD PTR [rbp-0x158]
mov    DWORD PTR [rax],0x1
call <usleep@plt>
mov    r11,QWORD PTR [rbp-0x150]
mov    r10d,DWORD PTR [rbp-0x130]
jmp <render_thread_loop+0x610>
call <__stack_chk_fail@plt>
cs nop WORD PTR [rax+rax*1+0x0]

