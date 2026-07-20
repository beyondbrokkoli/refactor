 i vibecoded an 8 player rollback netcode lockstep isometric vulkan glfw engine. it only uses only one vkDeviceWaitIdle and that is when shutting down the app. resizing and closing tenants, yes its a MAX_WINDOWS = 4 multiplexer that is able to render independent camera views on the isometric grid per tenant and routing clicks succesfully also per tenant, elevating colored isometric tiles ... all participants basically have  4 independent views into the lockstep arena , the matchmaking process connects to hetzner and figures out the optimal ICE. STUN hole punching or relay for cellular net. all ticks are cryptographically verified and UDP allows us to lookahead 61 ticks, where we wait on the laggiest opponent  when having terrible connection. all out of order clumsy windows network jitter simulation with concurrent tc scripts with heavy lag and out of order and duplicates ... nothing can desync the game. and now at last we refined the resize and teardown of the tenants to use a simple wsi flip trick with a double buffer that naturally gc's the zombie swapchains via epoch based mem free


That sounds like a substantial engineering effort. If the architecture works the way you describe, you've ended up solving a lot of the problems that typically cause engines to become complicated over time.
A few aspects stand out:

No runtime vkDeviceWaitIdle() is a strong design goal. If all synchronization is done through fences, timeline semaphores, per-frame resources, and deferred destruction, then only waiting at shutdown is exactly what many Vulkan developers aim for.
Multiple independent GLFW windows ("tenants") with their own cameras, input routing, and swapchains is harder than it first appears. Resize and teardown logic often become the messiest part of a renderer.
Epoch-based destruction for swapchains is a clean solution. Instead of trying to determine exactly when every image view, framebuffer, and swapchain-dependent object is no longer in flight, you let GPU progress naturally advance epochs before reclaiming resources.
Rollback lockstep with deterministic simulation plus cryptographic tick verification provides a strong mechanism for detecting divergence. The verification doesn't prevent desyncs by itself, but it makes them immediately detectable and attributable.
Testing under packet duplication, reordering, jitter, and delay is much more convincing than only testing on a clean LAN. Many networking bugs don't appear until packets arrive in pathological orders.
ICE selection with STUN or relay fallback is the practical approach for consumer networks, especially when mixing home broadband and cellular users.
The resize trick you mentioned is particularly interesting. Many Vulkan engines rebuild the swapchain immediately and then spend a lot of code carefully tracking which old objects are still referenced by submitted command buffers. If you're instead treating every resize as a "flip" to a fresh generation and letting the old generation die naturally once its epoch retires, that tends to eliminate an entire class of lifetime bugs.
Waiting up to 61 ticks for the slowest peer is a design trade-off. It increases tolerance for poor connections, but it also increases worst-case input latency before rollback or stalling occurs. Whether that's the right number depends on your simulation rate and the experience you're targeting, but having a configurable lookahead window is generally useful.

The fact that you specifically mention:

duplicate packets,
out-of-order packets,
heavy jitter,
packet delay,
simultaneous network emulation,
suggests you were testing the networking layer rather than assuming correctness. That's often what distinguishes a networking system that survives real-world conditions from one that only works in ideal environments.
If all of this is implemented cleanly, you've built something that combines several fairly advanced subsystems:

a deterministic lockstep simulation,
rollback networking,
Vulkan resource lifetime management without runtime device-idle stalls,
multi-window rendering,
NAT traversal,
and deterministic stress testing under adverse network conditions.
One thing I'd recommend before considering it "finished" is running an automated endurance test: leave eight clients connected under continuous randomized input for 24–72 hours with ongoing packet loss, duplication, reordering, and periodic window creation, resizing, and destruction. Those long-duration runs are excellent at exposing rare lifetime, synchronization, or determinism issues that shorter tests can miss.
Overall, it's an ambitious combination of systems, and the design choices you described—particularly avoiding runtime vkDeviceWaitIdle() and using deferred, epoch-based cleanup for swapchain resources—are consistent with patterns used to keep complex Vulkan applications responsive and free of unnecessary global synchronization.


[  main.lua  ]

Thanks for sharing it. I obviously can't verify the whole engine from one file, but I can comment on the architecture, and there are some things I genuinely like.
1. This is an orchestrator, not a "God object"
At first glance it looks huge (~700+ lines), but it's mostly orchestration.
The simulation itself isn't here.

Instead it's wiring together modules:

boot sequence
networking
rollback
renderer
tenants
resize FSM
teardown FSM
The actual work is delegated to things like:
Pump.intercept_network(...)
FSM.tick_playing_state(...)
render_queue.PackFrame(...)
TenantRegistry.boot_tenant(...)
graphics_mod.Init(...)

That's much healthier than a monolithic main loop.
2. I like the explicit state machines
Instead of nesting resize logic you literally have
tenant.wsi_state == 0
tenant.wsi_state == 1
tenant.wsi_state == 2

which is essentially
Idle
 ↓
Waiting
 ↓
Building
 ↓
Idle

That's easy to reason about.
Likewise teardown has

alive

↓

kill requested

↓

pool purge

↓

logical destroy

↓

physical destroy

instead of trying to do everything immediately.
3. The zombies are actually a nice idea
Instead of
destroy swapchain NOW

you do
old graphics
old sync
timestamp

and later
if age > ...

destroy them.
That avoids the classic

"is this framebuffer still referenced by a command buffer?"
question entirely.
It's very similar to deferred destruction queues used in many engines.

4. No runtime DeviceWaitIdle()
That aligns with your earlier description.
The only place I found is effectively

kill threads

↓

join

↓

device idle

↓

destroy everything

That is exactly where a global wait belongs.
5. I love the diagnostic mentality
This stood out:
execute_heartbeat_diagnostic()

Instead of hoping rollback is correct you continuously ask
did we diverge?
and immediately dump
tick

hashes

commands

peer

timeline

before crashing.
That's how deterministic engines become trustworthy.

6. Multi-window routing is cleaner than expected
I expected a pile of globals.
Instead everything is

TenantRegistry.active[win]

then
tenant.cam

tenant.gfx

tenant.sync

tenant.sc

tenant.inv_vp

tenant.pc

Every window owns almost all of its own rendering state.
7. The boot coroutine is elegant
This:
AWAIT_SURFACE

is nicer than blocking.
The boot sequence becomes

Stage 1

↓

yield

↓

surface exists

↓

continue

That's surprisingly readable.
Things I'd probably revisit
There are only a few architectural things that caught my eye.
1. goto continue_tenant
Lua doesn't have continue, so I understand why.
Still, there are a lot of

goto continue_tenant

sprinkled around.
It works, but after enough branches it becomes harder to mentally follow.

2. Magic timings
Example
if age > 1.0

or
if age > 2.5

Those deserve named constants.
Something like

POOL_PURGE_DELAY

LOGIC_PURGE_DELAY

SURFACE_PURGE_DELAY

makes the teardown protocol self-documenting.
3. main() is still doing a lot
It's orchestrating correctly, but it's also around 700 lines.
Eventually I'd probably split into things like

SimulationPhase()

InputPhase()

ResizePhase()

ZombieGC()

RenderPhase()

ShutdownPhase()

not because the code is bad, but because it's becoming an engine kernel.
4. require(...) inside loops
I noticed things like
local graphics_mod = require("graphics_pipeline")

inside resize code.
Lua caches require, so this isn't a performance issue.

Still, stylistically I'd move those requires to the top unless you're intentionally using lazy loading.

Overall impression
The biggest surprise is that this doesn't read like "vibe-coded spaghetti."
It actually has a fairly coherent philosophy:

explicit ownership
explicit state machines
deferred destruction
deterministic simulation
modular subsystems
aggressive runtime diagnostics
very little hidden control flow
That's not something that accidentally emerges from random prompting. Even if AI helped write portions, someone still had to decide the architecture and keep all the pieces consistent.
If I were reviewing this as an engine prototype, I wouldn't spend my first hour criticizing the code. I'd spend it asking questions like:

How does the rollback arena manage snapshots?
How does render_queue.PackFrame() avoid synchronization hazards?
What's the exact ownership model between the Lua orchestrator and the C Vulkan backend?
How are cryptographic tick hashes computed and exchanged?
How does your epoch system decide when a resource generation is safe to reclaim?
Those are the parts I'd be most interested in understanding next.
