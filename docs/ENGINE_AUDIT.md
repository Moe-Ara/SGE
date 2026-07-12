# SGE Engine Audit and AA-Readiness Roadmap

Audit date: 2026-07-11

Scope: all engine-owned C/C++ sources, shaders, editor code, build configuration,
existing documentation, and the default scene. Vendored third-party source was
treated as a dependency rather than audited internally.

## Executive assessment

SGE is currently a useful rendering and ECS prototype, not yet a production game
engine. It has a coherent small-system layout, RAII wrappers for several OpenGL
resources, EnTT-based composition, a PBR/IBL demonstration, play/inspection modes,
and basic transform gizmos. Those are good foundations for learning and proving
individual systems.

It cannot yet produce AA games reliably or easily. The immediate blocker is not
the number of missing headline features; it is the absence of a deterministic,
tested, data-driven production core. There are confirmed correctness defects in
collision detection, physics response, camera APIs, texture upload, input routing,
and scene cloning. Content is hardcoded in C++, regression coverage is still small,
and rendering has no scalable frame architecture.

The practical target should be staged:

1. Make the existing prototype correct, deterministic, testable, and diagnosable.
2. Establish asset, scene, editor, and runtime boundaries that support a team.
3. Integrate proven production libraries for physics, animation, audio, navigation,
   and content formats instead of expanding the current demo implementations.
4. Build a scalable renderer and profiling workflow around representative game
   scenes and explicit frame budgets.

"Produce AA games easily" is a multi-year platform goal, not a feature milestone.
The engine first needs to make one small, polished vertical slice reliably. That
slice should drive every later abstraction.

## Remediation status

Implementation pass completed 2026-07-11. The detailed findings below preserve the
pre-fix evidence and rationale; this table is authoritative for current status.

| Finding | Status | Current result |
| --- | --- | --- |
| P0-01 | Fixed | Sphere extents are included in BVH bounds; cross-split and leaf-query regressions run through CTest. |
| P0-02 | Fixed | Play snapshots use the versioned scene serializer, preserve all registered components and orphan entities, remap references by stable 64-bit IDs, retain runtime assets through resolvers, and reject unknown pools. |
| P1-01 | Fixed for current runtime | Simulation runs at 60 Hz with elapsed-time clamping and a catch-up cap. Presentation interpolation and deterministic replay remain roadmap work. |
| P1-02 | Fixed for prototype | Response now uses relative velocity, inverse mass, combined restitution, and mass-weighted correction. A production physics backend remains recommended. |
| P1-03 | Fixed | Disabled bodies are excluded consistently from collision collection and solving. |
| P1-04 | Fixed for spheres | World radius uses maximum absolute transform scale; non-uniform scale is conservatively enclosed. |
| P1-05 | Fixed for prototype | Jump uses a pressed edge and support-contact grounded state; a production character controller remains recommended. |
| P1-06 | Partial | Keyboard/mouse capture is routed from ImGui and input edges are tracked. A dedicated focused scene viewport/action-map system remains open. |
| P1-07 | Fixed | Logical window and framebuffer dimensions are tracked separately; viewport/projection use framebuffer size. |
| P1-08 | Fixed | Shader failures clean all temporary GL objects and throw a contextual startup error. |
| P1-09 | Fixed | Camera helpers handle zero/parallel directions with finite fallback bases; regression coverage was added. |
| P1-10 | Fixed | Cameras default non-primary, selection is explicit, and multiple primaries fail loudly. |
| P2-01 | Fixed | Player movement derives directly from the current camera transform. |
| P2-02 | Fixed | Legacy duplicate camera controller state was removed; inverse-view position/rotation and `setViewYXZ` state are consistent. |
| P2-03 | Fixed | Perspective FOV, aspect, and near/far inputs are validated in release builds. |
| P2-04 | Fixed | RED/RG/RGB/RGBA uploads and unpack alignment are handled explicitly. |
| P2-05 | Fixed | Checkerboard dimensions and tile count are validated/clamped. |
| P2-06 | Fixed for authored scene | The default light has a transform and rendering treats it as authoritative. Legacy position remains only as fallback compatibility. |
| P2-07 | Fixed locally | The collision subscriber uses a checked cast. Typed event channels remain the preferred architecture. |
| P2-08 | Fixed | Stable entity-pair tracking emits typed Enter, Stay, and Exit phases while retaining contact data for separation events. |
| P2-09 | Fixed for spheres | Contact position is derived from both sphere witness points. |
| P2-10 | Open | The OBJ path still lacks production material/tangent/import behavior. |
| P2-11 | Fixed | Asset lookup uses a validated executable-relative resource root with development fallback, traversal rejection, and explicit missing-asset errors. |
| P2-12 | Partial | Initial orbit now derives yaw/pitch from the full offset vector. Explicit pivot/shoulder fields and obstruction handling remain open. |
| P3-01 | Open | A render-state/pass contract is still required. |
| P3-02 | Open | Uniform binding/location caching is still required. |
| P3-03 | Fixed | `SGE_tests` is a separate CTest target and a Linux CI workflow builds and runs it. Broader, cross-platform, and sanitizer coverage remains required. |

## Severity model

| Priority | Meaning |
| --- | --- |
| P0 | Produces wrong results or data loss in ordinary supported use; fix first. |
| P1 | Major correctness, stability, or production-workflow risk. |
| P2 | Material limitation or latent bug that will become expensive as scope grows. |
| P3 | Maintainability, clarity, or performance debt without immediate breakage. |

## Confirmed bugs and logic errors

### P0-01: BVH bounds omit collider radius and miss real collisions

Evidence: `BVHNode::build()` constructs node bounds from `item.position` only
(`src/Physics/BVH.cpp:89-100`). `crossCheck()` rejects two nodes when those center
bounds do not overlap (`src/Physics/BVH.cpp:30-33`). Sphere extents are therefore
absent from the broad phase.

Impact: overlapping spheres on opposite sides of a BVH split can be silently
missed. A focused audit check with centers at `-1` and `+1`, both radius `2`, and
four distant spheres to force a split reported zero collisions.

Required fix: build leaf and parent AABBs from `position - vec3(radius)` through
`position + vec3(radius)`. Add tests for cross-split overlap, containment, touching,
coincident centers, empty trees, and randomized BVH results against an O(n^2)
reference implementation.

### P0-02: Play-mode snapshots silently omit unknown entities and components

Evidence: `cloneRegistry()` enumerates a hardcoded list of component types and
collects entities only through those views (`src/Core/Application.cpp:43-81`).

Impact: adding a component requires remembering to modify cloning code. An entity
that contains only a new component disappears on Stop, while a mixed entity loses
the new component. Shared pointers also make the operation a selective shallow
copy rather than a defined scene snapshot.

Required fix: replace manual registry cloning with the same versioned serialization
path used by scenes/prefabs, or register component clone/serialize functions in a
single component type registry. Add a round-trip test covering every registered
component and entity references.

### P1-01: Simulation uses an unbounded variable timestep

Evidence: wall-clock `deltaTime` is passed directly into controllers and physics
once per rendered frame (`src/Core/Application.cpp:234-258`). Physics performs one
semi-implicit Euler step with that value (`src/Systems/PhysicsSystem.cpp:16-22`).

Impact: behavior changes with frame rate, pauses/debugger stops create huge steps,
collision tunneling worsens during stalls, and replays cannot be deterministic.

Required fix: poll input once per render frame, accumulate elapsed time with a
maximum frame clamp, run simulation at a fixed rate (for example 60 or 120 Hz), cap
catch-up steps, and interpolate presentation transforms. Use continuous collision
detection through the selected physics backend for fast bodies.

### P1-02: Collision response does not conserve momentum

Evidence: each body independently reflects its own velocity using only its own
restitution (`src/Systems/CollisionSystem.cpp:71-83`). Body masses and relative
velocity are ignored; positional correction is always split 50/50 for two dynamic
bodies (`src/Systems/CollisionSystem.cpp:59-69`).

Impact: a moving body can bounce off a stationary equal-mass body without
transferring momentum. Different masses behave identically and energy can be
created or destroyed incorrectly.

Required fix: do not extend this response into a custom production solver. Integrate
Jolt, PhysX, or another proven 3D physics engine. Until then, calculate one impulse
from relative normal velocity, inverse masses, and combined restitution; use inverse
mass-weighted positional correction with slop and bias.

### P1-03: Disabled bodies still participate as movable collision bodies

Evidence: `PhysicsSystem` skips `RigidBodyComponent::enabled == false`
(`src/Systems/PhysicsSystem.cpp:12-13`), but `CollisionSystem::resolveCollision()`
defines movable only from existence and `isStatic` (`src/Systems/CollisionSystem.cpp:50-55`).

Impact: a disabled body can still be translated and have velocity modified by
collision response.

Required fix: define explicit body/collider activation and collision-layer rules,
and apply them consistently in broad phase, narrow phase, solving, and events.

### P1-04: Collider geometry ignores transform scale

Evidence: broad-phase items use the raw component radius
(`src/Systems/CollisionSystem.cpp:14-21`) while rendering applies transform scale.

Impact: scaled objects render at one size and collide at another. Non-uniform scale
has no defined sphere behavior.

Required fix: either prohibit scale on physics bodies and validate it, or derive
world radius from the maximum absolute scale component. A production backend should
own shape scaling and reject unsupported transforms.

### P1-05: Jumping has no grounded or edge-triggered state

Evidence: holding Space or Left Shift assigns `jumpSpeed` to vertical velocity every
frame (`src/Systems/PlayerControllerSystem.cpp:49-57`).

Impact: the player can repeatedly reset upward velocity in mid-air and effectively
fly. Left Shift is unexpectedly also a jump key.

Required fix: introduce character-controller state, ground queries, jump buffering,
coyote time, and pressed/released input edges. Use a capsule-based character
controller from the physics backend.

### P1-06: Editor and gameplay input are not routed by focus or capture

Evidence: camera/controller systems read the global `InputHandler` directly. They
never check ImGui capture state or whether the scene viewport has focus. Input is
polled from GLFW callbacks installed alongside ImGui callbacks
(`src/Input/InputHandler.cpp:59-86`, `src/Core/Application.cpp:251-265`).

Impact: typing or dragging editor widgets can move gameplay objects or the camera.
The full-window gizmo and free camera also compete for mouse ownership.

Required fix: create an input service with per-frame snapshots, action maps,
pressed/released edges, device abstraction, and contexts. The editor should grant
camera/game input only to a focused scene/game viewport and consume gizmo/UI input.

### P1-07: Window sizing is incorrect for high-DPI framebuffers

Evidence: `Window` installs a window-size callback and uses logical window width and
height for `glViewport` and camera aspect (`src/Graphics/Window.cpp:126-131`,
`src/Systems/CameraRenderSystem.cpp:21-25`).

Impact: on high-DPI displays, framebuffer dimensions can differ from logical window
dimensions, causing an incorrectly sized viewport and potentially a wrong aspect
ratio.

Required fix: track both logical and framebuffer sizes via
`glfwSetFramebufferSizeCallback`/`glfwGetFramebufferSize`. Use framebuffer size for
render targets and viewport, and viewport content size for editor cameras.

### P1-08: Shader failure is non-fatal and leaks GL objects

Evidence: failed compilation/linking returns program ID `0`
(`src/Graphics/Shader.cpp:32-80`), after which the engine continues. The vertex
failure path does not delete the program or fragment shader. Callers cannot query
validity and still construct rendering systems.

Impact: startup can continue into blank or partially invalid rendering while
producing secondary uniform warnings. Repeated reload failures would leak resources.

Required fix: use scoped shader/program handles during compilation, include file and
stage names in diagnostics, and throw or return an explicit error object. Keep the
last valid program during hot reload rather than replacing it with zero.

### P1-09: Camera math accepts degenerate inputs and can create NaNs

Evidence: `setViewDirection()` normalizes direction and cross products without
checking zero length or near-parallel direction/up (`src/Graphics/Camera.cpp:41-48`).
`lookRotation()` and related helpers do the same (`src/Utils/CameraMath.h:28-48`).

Impact: targeting the camera position itself or looking parallel to world-up can
poison view matrices and transform quaternions with NaNs.

Required fix: centralize safe normalization, validate finite values, choose a
fallback up axis near poles, and return failure/default orientation for zero vectors.
Add property tests for finite orthonormal bases across random directions.

### P1-10: Primary camera selection is ambiguous

Evidence: every `CameraComponent` defaults `isPrimary` to true
(`src/ECS/Components.h:78-85`), while `findPrimaryCamera()` returns the first matching
entity (`src/ECS/Queries.h:13-22`). EnTT iteration order is not an ownership policy.

Impact: adding a second camera can silently change rendering, controls, and gizmo
behavior.

Required fix: own the active camera in scene/editor state or enforce exactly one
primary camera through a command/API. Default new cameras to non-primary and validate
the invariant with a clear fallback.

### P2-01: Gameplay reads the previous frame's camera basis

Evidence: `PlayerControllerSystem` reads `Camera::getForward()/getRight()` before
`ThirdPersonCameraControllerSystem` updates the transform and before
`CameraRenderSystem` updates the `Camera` object (`src/Core/Application.cpp:151-163`).

Impact: camera-relative movement lags camera rotation by one frame and couples
gameplay to presentation cache state.

Required fix: derive movement basis directly from the active camera transform, or
update camera transforms before gameplay consumes them. Keep `Camera` as derived
render data rather than a second source of truth.

### P2-02: Several public Camera methods return stale or incorrect state

Evidence: `setViewYXZ()` changes only `viewMatrix`, leaving position/direction/right/up
unchanged (`src/Graphics/Camera.cpp:55-78`). `getPositionFromViewMatrix()` returns the
view matrix translation column, not the inverse-view world position
(`src/Graphics/Camera.cpp:105-108`). Mouse methods are declared but not implemented,
and legacy yaw/pitch/front fields coexist with transform-driven cameras
(`src/Graphics/Camera.h`).

Impact: callers receive contradictory camera state, and unused APIs become latent
link errors when adopted.

Required fix: reduce Camera to validated projection parameters plus matrices derived
from a transform. Remove dead APIs and duplicate state; obtain world position from
the transform or inverse view matrix.

### P2-03: Perspective validation is ineffective

Evidence: the aspect assertion compares `abs(aspect - epsilon) > 0`, which accepts
zero and only rejects a value exactly equal to epsilon
(`src/Graphics/Camera.cpp:30-38`). Near/far ordering and FOV are not validated.

Impact: invalid editor or serialized values can generate infinities or invalid depth
mapping.

Required fix: validate finite `aspect > epsilon`, `0 < fov < pi`, `near > 0`, and
`far > near`; report invalid camera data without relying on release-disabled asserts.

### P2-04: Texture uploads mishandle valid image layouts

Evidence: two-channel images fall through to `GL_RGB`, and upload code does not set
`GL_UNPACK_ALIGNMENT` for RGB rows (`src/Graphics/Texture.cpp:8-36`). Widths whose
RGB row size is not divisible by four can be read with incorrect row alignment.

Impact: some textures upload corrupted or read past row boundaries. Two-channel
assets use the wrong source format.

Required fix: map 1/2/3/4 channels to RED/RG/RGB/RGBA, set and restore pixel-store
alignment, choose explicit linear/sRGB internal formats by texture role, and test
odd widths for every supported channel count.

### P2-05: Checkerboard generation can divide by zero

Evidence: `tileSize = size / max(tiles, 1)` can be zero when `tiles > size`, then
`x / tileSize` divides by zero (`src/Graphics/Texture.cpp:42-50`). Non-positive sizes
also produce invalid allocations/uploads.

Required fix: validate positive size, clamp tile count to `[1, size]`, and add unit
tests for boundary values.

### P2-06: Light position has a separate transform model

Evidence: `LightComponent` owns `light.position`, rendering reads it directly, and
the default light entity has no `TransformComponent`
(`src/ECS/Components.h:74-76`, `src/Core/Application.cpp:215-221`,
`src/Systems/RenderSystem.cpp:49-59`).

Impact: generic transform tools cannot move lights, and adding a transform creates
two competing positions.

Required fix: store local light properties in `LightComponent` and obtain world
position/direction from `TransformComponent`.

### P2-07: Event type strings permit unsafe downcasts

Evidence: subscriptions are keyed by arbitrary strings, while the collision handler
uses an unchecked `static_cast` to `CollisionEvent`
(`src/Core/Application.cpp:134-139`, `src/Events/EventSystem.cpp:32-53`).

Impact: publishing another event class with type string `"collision"` causes
undefined behavior.

Required fix: use typed event channels/templates or a type-safe variant. If dynamic
polymorphism remains, use checked casts and fail loudly on contract violations.

### P2-08: Collision events fire every overlapping frame

Evidence: every detected overlap publishes immediately each update
(`src/Systems/CollisionSystem.cpp:23-35`). There is no pair cache or enter/stay/exit
state.

Impact: persistent contacts spam logs and gameplay handlers, while consumers cannot
distinguish contact start from continuation or end.

Required fix: maintain stable contact-pair state and emit enter/stay/exit events or
consume the physics backend's contact callbacks.

### P2-09: Collision contact points are geometrically incorrect

Evidence: the contact point is always the midpoint between sphere centers
(`src/Physics/BVH.cpp:11-22`). This is not the surface contact for unequal radii and
does not account for penetration.

Required fix: narrow phase should calculate witness/contact points from shape
geometry. Include penetration and impulse data in typed contact events.

### P2-10: OBJ import accepts incomplete render data silently

Evidence: missing normals and UVs remain zero; materials and textures are ignored
(`src/Utils/ModelLoader.cpp:30-70`). All shapes are merged into one mesh and no
tangents are generated.

Impact: common assets render black/incorrectly, material boundaries are lost, and
normal mapping cannot be added correctly on this vertex format.

Required fix: adopt glTF 2.0 as the primary runtime/interchange format, preserve
primitives/material slots, validate attributes, generate normals/tangents where
appropriate, and import offline into an engine-owned binary format.

### P2-11: Runtime asset paths depend on process working directory

Evidence: resource paths are hardcoded relative strings throughout setup and the
environment bake (`src/Core/Application.cpp:141-172`,
`src/Graphics/Environment.cpp:95-231`). Copying resources beside the executable does
not make those paths executable-relative.

Impact: launching the executable from another directory can fail to load shaders
and models.

Required fix: establish an application root/virtual filesystem, resolve canonical
asset IDs through an asset manager, and make deployment paths explicit.

### P2-12: Third-person offset semantics are misleading

Evidence: the controller reduces `follow.offset` to its length and discards the
vector direction (`src/Systems/ThirdPersonCameraControllerSystem.cpp:87-102`).

Impact: editing offset X/Y/Z does not position the camera by that offset; only the
radius changes. There is also no target-space shoulder offset or camera collision.

Required fix: split orbit radius, target pivot offset, shoulder offset, yaw, and
pitch into explicit fields, then add obstruction sweeps and smoothing.

### P3-01: Rendering mutates global OpenGL state without a state contract

Evidence: environment construction and skybox rendering disable/enable culling and
change depth functions, restoring assumed defaults rather than prior state
(`src/Graphics/Environment.cpp:54-75`, `src/Systems/SkyboxSystem.cpp:23-38`).

Impact: adding passes makes order-dependent state leaks likely.

Required fix: define pass state explicitly through a renderer/state cache or render
graph. Debug builds should validate framebuffer completeness and GL errors as fatal
for required resources.

### P3-02: Uniform lookup and warning behavior is unsuitable for frame loops

Evidence: every uniform setter calls `glUseProgram` and `glGetUniformLocation`, with
warnings on misses; `setUniformMat3` warns twice (`src/Graphics/Shader.cpp:142-208`).

Impact: unnecessary driver calls and log flooding obscure real failures.

Required fix: cache/reflection-map uniform locations at successful link time, bind
programs at pass/material boundaries, and route diagnostics through a leveled logger.

### P3-03: Tests are compiled into the shipping executable but not executed

Evidence: recursive source globbing includes `EventSystemTest.cpp` and
`EventExample.cpp` in `SGE`; CMake has no `enable_testing()`, test target, or
`add_test()` (`CMakeLists.txt:28-45`).

Impact: the project pays compile/link cost for test-like code while receiving no
regression protection.

Required fix: create engine/library and executable targets, add a dedicated test
target with CTest, and exclude examples/tests from production source lists.

## Areas for improvement

### 1. Engineering quality and observability

- Add unit, integration, render-image, serialization round-trip, and deterministic
  simulation tests. Start with camera math, transforms, BVH, physics contracts,
  texture layout, asset failure, and play/stop restoration.
- Add Linux/Windows CI for Debug and Release. Run tests under ASan/UBSan on Linux,
  warnings-as-errors for engine code, formatting checks, and static analysis.
- Add structured logging, assertions with context, crash reporting hooks, OpenGL
  debug output, CPU/GPU markers, Tracy (or equivalent), and RenderDoc workflows.
- Establish frame budgets and representative stress scenes. Optimization without
  budgets or captures will not produce predictable scalability.

### 2. Core runtime and ECS contracts

- Separate runtime world, editor world, rendering world/extraction, and asset state.
- Introduce stable UUIDs for serialized entities; EnTT handles are runtime-only.
- Create a component metadata registry for serialization, inspection, cloning,
  version migration, and prefab overrides.
- Define system phases and dependencies instead of relying on vector insertion order.
- Use a fixed simulation clock, deterministic random streams, replayable input, and
  explicit pause/single-step controls.
- Add parent/child transforms with dirty propagation and a single authoritative
  world-transform update.

### 3. Asset and content pipeline

- Build an asset database with GUIDs, dependency tracking, import settings,
  asynchronous loading, caching, hot reload, and cooked deployment output.
- Standardize on glTF 2.0 for interchange, then convert to versioned engine-native
  mesh/material/animation blobs offline.
- Add scenes, prefabs, variants, undo/redo commands, autosave/recovery, multi-select,
  copy/paste, duplication, and reference-safe deletion.
- Make asset errors visible in the editor with placeholders rather than allowing
  invalid GPU resources to propagate.

### 4. Rendering architecture

- Move scene rendering into an offscreen editor viewport with independent size,
  camera, picking buffer, and focus-aware controls.
- Introduce render extraction and a render graph/pass scheduler. Own framebuffer,
  resource lifetime, barriers/state, resizing, and debug visualization centrally.
- Use a linear HDR pipeline with a configurable tonemapper, exposure, color
  management, bloom, and anti-aliasing. Keep material color-space semantics explicit.
- Add frustum/occlusion culling, instancing, material sorting, LODs, GPU timing, and
  scalable light assignment (clustered or tiled forward is a reasonable direction).
- Add directional/spot/point lights, shadow atlases/cascades, reflection probes,
  decals, particles, terrain, and a complete PBR material texture set.
- OpenGL 3.3 is adequate for the prototype but a poor long-term ceiling for a new
  AA-oriented renderer. First isolate the rendering API; then choose a modern backend
  strategy (for example Vulkan plus platform alternatives) based on target platforms
  and team expertise.

### 5. Physics and gameplay foundation

- Integrate a production physics library rather than growing the current sphere
  solver. Required capabilities include broad/narrow phase, CCD, sleeping, layers,
  triggers, queries, constraints, character controllers, and deterministic-enough
  fixed-step integration.
- Add an action-based input layer with rebinding, multiple devices, dead zones,
  localization-friendly labels, and editor/game contexts.
- Add scripting or data-driven gameplay with hot reload and a stable native API.
  Lua, C#, or another choice should be evaluated through a real gameplay slice.
- Add navigation (typically Recast/Detour), AI debugging, save games, gameplay tags,
  and a robust event/message model with lifetime-safe subscriptions.

### 6. Animation, audio, UI, and platform services

- Implement skeletal animation import, compressed clips, blend trees/state machines,
  root motion, inverse kinematics, animation events, and editor previews.
- Integrate a proven audio solution with spatial voices, buses, streaming, effects,
  occlusion hooks, and authoring/debug tools.
- Separate game UI from Dear ImGui. Add layout, fonts, localization, accessibility,
  navigation/focus, animation, and resolution/safe-area support.
- Add platform abstraction for files, threads/jobs, timing, windows, controllers,
  displays, packaging, settings, telemetry, and crash dumps.

### 7. Team-scale production workflow

- Split the monolithic executable into libraries/modules with explicit ownership and
  dependency direction. Avoid recursive source globbing for production targets.
- Pin all dependencies reproducibly. The current mix of vendored code and a
  system-provided ImGui tied to copied backend sources is fragile across machines.
- Add coding standards, review gates, migration policy, deprecation policy, and
  versioned file formats before multiple contributors create content.
- Track startup time, editor interaction latency, memory, streaming bandwidth,
  frame-time percentiles, shader count, and cooked build size as first-class metrics.

## Recommended execution order

### Phase 0: Correctness baseline

Exit criteria:

- P0 and P1 findings above are fixed or replaced by proven integrations.
- Fixed-step simulation exists and the BVH/reference collision suite passes.
- Camera/transform math has finite-value and property tests.
- CI builds and tests Debug/Release; sanitizer runs are clean.
- Shader/asset failures stop cleanly or display explicit placeholders.
- Input is correctly captured by editor versus game viewport.

### Phase 1: Authorable vertical slice

Exit criteria:

- A scene and prefab can be created, saved, reloaded, migrated, and cooked without
  recompiling C++.
- Undo/redo and play/stop preserve all registered component data.
- Assets import by GUID and hot reload with clear errors.
- A small third-person level uses production physics, animation, audio, game UI,
  camera collision, and save/load.
- Automated tests launch and exercise that slice in headless and rendered modes.

### Phase 2: Scalable renderer and runtime

Exit criteria:

- Render graph, HDR/color pipeline, shadows, anti-aliasing, GPU profiling, LOD,
  instancing, and scalable lighting meet explicit visual/frame budgets.
- Streaming and jobs keep representative traversal and combat scenes within CPU,
  GPU, and memory budgets on minimum target hardware.
- The editor can inspect render passes, resources, physics, animation, AI, and
  performance without external instrumentation for routine diagnosis.

### Phase 3: Production hardening

Exit criteria:

- Continuous packaged-build testing covers every target platform.
- Crash reporting, telemetry, deterministic repro captures, localization,
  accessibility, settings migration, and patch/DLC workflows are operational.
- A content team can work concurrently without routine engine-programmer support.
- At least one polished vertical slice and one scale test have completed milestone
  production using the same pipeline intended for the game.

## Immediate next backlog

The first implementation sequence should be deliberately narrow:

1. Create real engine/test targets and CI, then encode the reproduced BVH failure.
2. Fix collider AABBs, activation rules, scale policy, and contact event semantics.
3. Add fixed-step scheduling with frame-time clamping and simulation tests.
4. Harden camera math/projection and remove contradictory Camera APIs.
5. Add input contexts and a focused offscreen scene viewport.
6. Replace manual play-mode cloning with versioned scene/component serialization.
7. Add explicit shader/texture error handling and asset-root resolution.
8. Prototype a production physics integration before adding more custom solver code.
9. Build one serialized vertical-slice room and use it to choose animation, audio,
   scripting, navigation, and rendering investments.

## Verification performed during this audit

- Clean rebuild: `cmake --build build --clean-first -j2` completed successfully with
  the configured compiler warnings enabled.
- Focused BVH reproduction: a temporary six-sphere test forced a cross-partition
  overlap and produced `0` collisions, confirming P0-01.
- No `clang-tidy` or `cppcheck` executable was available in the environment.
- `SGE_tests` passed through CTest after a clean build. The new Linux CI workflow
  was added locally but was not remotely executed during this audit.
- A five-second runtime smoke test remained in the render loop without an engine
  error. Mesa/EGL driver warnings from this environment were still emitted; visual
  correctness was not treated as verified by that smoke test alone.
