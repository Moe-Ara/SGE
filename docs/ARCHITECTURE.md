# SGE Architecture

> This document describes the intended architecture. The newer
> [engine audit](ENGINE_AUDIT.md) is authoritative for current defects and the
> production-readiness roadmap. In particular, the audit includes a reproduced
> cross-partition BVH failure that supersedes older claims in this document.

This document explains how the engine is put together, how to extend it, and what is
still missing before it could support a real (AA-scale) game. It reflects the
2026-07 rewrite: the engine moved from a hand-rolled `GameObject`/`Actor` inheritance
hierarchy to a data-oriented ECS (entity-component-system) built on
[EnTT](https://github.com/skypjack/entt), rendering moved from GLEW to
[GLAD](https://glad.dav1d.de/), and an in-process [Dear ImGui](https://github.com/ocornut/imgui)
debug/editor overlay was added.

If you're picking this project back up after a break, read this file top to bottom
once — it will save you re-deriving the architecture from the source.

## 1. Big picture

```
main() -> Application::run()
             |
             +-- Window::getInstance()      (GLFW window + GLAD-loaded GL context, singleton)
             +-- entt::registry             (all game state lives here as components)
             +-- InputHandler               (polls GLFW key/mouse state)
             +-- EventSystem                (pub/sub, e.g. collision events)
             +-- Environment                 (baked once at startup — see section 5b)
             +-- vector<unique_ptr<ISystem>>   (run in this order — see note below)
             |     InputSystem    -> input stage; InputHandler owns GLFW key/mouse state
             |     PlayerControllerSystem -> maps player input to velocity/translation
             |     FreeCameraControllerSystem -> moves FreeCameraComponent cameras
             |     ThirdPersonCameraControllerSystem -> moves follow cameras around targets
             |     PhysicsSystem  -> gravity + velocity/position integration
             |     CollisionSystem-> BVH broad-phase + sphere narrow-phase, resolves overlaps, publishes events
             |     CameraRenderSystem -> syncs CameraComponent view/projection from TransformComponent
             |     RenderSystem   -> draws every entity with Transform+Mesh via the PBR shader + IBL ambient
             |     SkyboxSystem   -> draws the baked sky cubemap into whatever pixels are still empty
             +-- EditorUI                   (Dear ImGui: entity list + component inspector)
```

Every frame (`Application::gameLoop`, `src/Core/Application.cpp`):
1. Check for Escape (quit).
2. Compute `deltaTime`.
3. `window.clear()`.
4. Run every system in order over the registry.
5. `EditorUI::beginFrame()/draw()/endFrame()` — drawn last so it overlays the 3D scene.
6. `window.update()` — polls GLFW events and swaps buffers.

**Design principle:** components are plain data, systems are the only place behavior
lives, and `Application` just owns the registry and runs systems in a fixed order. This
is what makes the SOLID properties hold:
- **SRP** — each system does exactly one job.
- **OCP** — new behavior = new component + new system; you don't edit existing classes.
- **LSP** — there's no inheritance hierarchy of "actors" to subclass badly (the old
  `Player`/`npc` classes silently no-op'ing a required `render()` override is exactly
  the bug class this eliminates).
- **ISP** — an entity only carries the components it needs (an NPC has no
  `PlayerControllerComponent`; a light has no `MeshComponent`).
- **DIP** — `Application` depends on the `ISystem` abstraction, not concrete system
  types, so systems can be reordered/added/removed in one place
  (`Application::setup()`).

## 2. Directory map

```
external/            vendored third-party code (not written by us, don't "fix" the style)
  glad/               generated GL 3.3 core loader (gl.h + gl.c)
  entt/entt/entt.hpp  single-header ECS
  imgui_backends/     GLFW + OpenGL3 Dear ImGui backend, copied from the installed
                      libimgui-dev package version (must stay version-matched to the
                      system imgui.h/libimgui.a — see section 6)

src/
  Core/               Application: owns the registry, systems, editor, game loop
  ECS/
    Components.h       every component struct
    ISystem.h           the one-method system interface
  Systems/            one .h/.cpp pair per system (see section 4)
  Editor/             EditorUI (Dear ImGui panels)
  Graphics/           Window (GLFW+GLAD), Camera (pure view/projection math), Shader,
                      Texture (GL_TEXTURE_2D wrapper: stb_image file loading + a
                      procedural checkerboard generator used as the demo scene's
                      fallback/proof-of-pipeline texture)
  GameObjects/        Model (VAO/VBO/EBO wrapper) — despite the folder name, this no
                      longer has anything to do with "GameObject" as a class; it's
                      just where mesh GPU-resource code lives (see section 6, gap list)
  Physics/            BVH/AABB spatial structure (entity+position+radius payload, no
                      GameObject coupling)
  Events/             EventSystem (token-based pub/sub), CollisionEvent
  Input/              InputHandler (GLFW key/mouse polling)
  Utils/              ModelLoader (tinyobjloader wrapper), Material, Light, file_reader

resources/
  models/             .obj assets (loaded via tinyobjloader)
  shaders/            vertex.vert/fragment.frag (Blinn-Phong, currently unused by
                      default — see section 5) and pbr.vert/pbr.frag (the active
                      pipeline)
```

`CMakeLists.txt` globs all of `src/**/*.cpp` plus the vendored `external/` sources it's
told about explicitly. Dropping a new `.h`/`.cpp` pair anywhere under `src/` is picked
up automatically on the next `cmake` configure — you don't need to edit CMakeLists.txt
to add a new component or system file.

## 3. The component set (`src/ECS/Components.h`)

| Component | Fields | Used by |
|---|---|---|
| `TransformComponent` | `translation, rotation (quat), scale` + `mat4()`/`normalMatrix()` helpers | everything with a position |
| `MeshComponent` | `shared_ptr<Model>` | RenderSystem |
| `MaterialComponent` | `albedo, metallic, roughness, ao` (PBR) + optional `albedoTexture` (`shared_ptr<Texture>`, overrides `albedo` when set) + a `phong` `Material` (kept for the Blinn-Phong shader, unused by default) | RenderSystem |
| `RigidBodyComponent` | `velocity, acceleration, force, mass, restitution, isStatic, enabled` | PhysicsSystem |
| `SphereColliderComponent` | `radius` | CollisionSystem |
| `LightComponent` | wraps `::PointLight` (`Utils/Light.h`) | RenderSystem |
| `CameraComponent` | `GRAPHICS::Camera camera; bool isPrimary` | CameraRenderSystem, controller systems, RenderSystem |
| `ThirdPersonFollowComponent` | `target (entt::entity), offset, yaw, pitch, ...` | ThirdPersonCameraControllerSystem |
| `FreeCameraComponent` | `movementSpeed, sprintMultiplier, mouseSensitivity, yaw, pitch, ...` | FreeCameraControllerSystem |
| `PlayerControllerComponent` | `movementSpeed, jumpSpeed` | PlayerControllerSystem |
| `TagComponent` | `name` (display name) | EditorUI |

Components are intentionally dumb structs — if you find yourself adding a method with
real logic to one, that logic almost always belongs in a system instead.

## 4. Systems (`src/Systems/`)

All systems implement `ECS::ISystem::update(entt::registry&, float deltaTime)` and are
run in this fixed order by `Application::setup()`/`gameLoop()`. **The order is load-bearing**,
not arbitrary: controller systems write ECS state, simulation systems settle it, and
render-prep systems consume the finished data for the frame.

1. **InputSystem** — the input stage. GLFW callbacks update `InputHandler`; gameplay
   systems consume that input through injected `InputHandler` references.
2. **PlayerControllerSystem** — reads `InputHandler` key state, finds the primary camera
   (via `ECS::findPrimaryCamera`, `src/ECS/Queries.h` — the single source of truth so
   `RenderSystem` can't pick a different "primary" camera than controller systems do)
   to get a forward/right basis. For a `TransformComponent+PlayerControllerComponent`
   entity that also has a `RigidBodyComponent`, input sets **desired velocity**
   (`RigidBodyComponent::velocity.x/z` from WASD, `.y` from Space/Shift) rather than
   writing position directly — `PhysicsSystem` is the only thing that ever integrates
   velocity into `TransformComponent::translation`, so the two systems can't fight over
   the same field. Entities with a `PlayerControllerComponent` but no `RigidBodyComponent`
   (e.g. a free-flying debug camera rig) still get direct `translateBy` movement.
3. **FreeCameraControllerSystem** — operates only on entities with
   `CameraComponent+TransformComponent+FreeCameraComponent`. It reads keyboard/mouse
   input, updates `FreeCameraComponent` yaw/pitch/history state, and writes
   `TransformComponent` translation/rotation. It never touches projection matrices or
   rendering.
4. **ThirdPersonCameraControllerSystem** — operates only on entities with
   `CameraComponent+TransformComponent+ThirdPersonFollowComponent`. It reads optional
   right-mouse orbit input, updates follow yaw/pitch/history state, and writes the
   camera entity's `TransformComponent` around its target. It never writes directly to
   `GRAPHICS::Camera`.
5. **PhysicsSystem** — for every `TransformComponent+RigidBodyComponent` that isn't
   static/disabled: apply gravity, `force -> acceleration -> velocity -> position`,
   clear forces. This is the single integration path; there is no other place velocity
   gets modified, which is what fixes the old bug where gravity was added to a force
   accumulator that nothing ever drained.
6. **CollisionSystem** — every frame, gathers all
   `TransformComponent+SphereColliderComponent` entities into a flat list, rebuilds a
   `PHYSICS::BVH` from scratch (`BVH::build`), and asks it for overlapping pairs
   (`BVHNode::getCollisions` does a same-leaf pairwise check *and* a dual-tree
   `crossCheck` across the leaf/leaf boundary at every internal node, so two items on
   opposite sides of a split still collide correctly). For each overlap:
   positionally separates the two entities along the contact normal proportional to
   penetration depth, applies a restitution-scaled velocity response to whichever side(s)
   have a non-static `RigidBodyComponent`, then publishes a `"collision"` `CollisionEvent`
   (carrying the two `entt::entity` values, contact point, normal, and an
   Enter/Stay/Exit phase) through the
   injected `EventSystem`. This is a basic response (no rotation/torque, no
   constraint solver) — see gap list for what a real physics story still needs.
   Rebuilding the BVH from scratch each frame is deliberate: the old BVH had incremental
   `insert`/`remove` paths that could silently drop or stale entities; for a scene of
   this size, a full rebuild is simpler and correct by construction. If you have
   thousands of colliders, this is the first thing to revisit (see gap list).
7. **CameraRenderSystem** — takes a `GRAPHICS::Window&` injected at construction (not a
   `Window::getInstance()` singleton lookup inside `update()`, precisely so it can't run
   before the singleton has been constructed with real dimensions by
   `Application::setup()`). Each frame it recomputes every `CameraComponent`'s
   perspective projection from the *current* `window.getWidth()/getHeight()` aspect
   ratio (so resizing the window doesn't stretch the image), then syncs every
   `CameraComponent+TransformComponent` entity's `GRAPHICS::Camera` view from its
   transform. It does not read input and contains no controller-specific logic.
8. **RenderSystem** — finds the primary camera (same `ECS::findPrimaryCamera` helper)
   and up to 4 lights, sets `view`/`projection`/`viewPos`/`lights[i].*`/`numLights`
   once, binds the injected `Environment`'s irradiance/prefilter/BRDF-LUT textures to
   texture units 1/2/3 (unit 0 is reserved for each entity's own `albedoMap`), then for
   every `TransformComponent+MeshComponent` sets `transform`/`normalMatrix` and the PBR
   material uniforms (falling back to a default white/dielectric material if the entity
   has no `MaterialComponent`) and draws.
9. **SkyboxSystem** — runs last. Finds the primary camera, draws the `Environment`'s raw
   captured sky cubemap as the background using a view matrix with translation stripped
   (`skybox.vert`'s `mat4(mat3(view))`) and a vertex trick that forces the sky's depth to
   the far plane (`clipPos.xyww`) with `glDepthFunc(GL_LEQUAL)`, so it only fills in
   pixels nothing else drew this frame — no overdraw of already-shaded geometry.

To add a new camera type, add a pure data component such as `OrbitCameraComponent`,
add a matching `OrbitCameraControllerSystem` that writes `TransformComponent`, and
register that system before `CameraRenderSystem`. Existing camera controllers and
`CameraRenderSystem` do not need to change.

To add any other system: implement `ISystem`, construct it in `Application::setup()`,
push it into the vector in the position you want it evaluated. That's the entire
integration surface — nothing else needs to know about it.

## 5. Rendering & shaders

`Shader` (`src/Graphics/Shader.h`) is a thin GLSL program wrapper: `use()`, generic
`setMat4/setMat3/setVec3/setFloat/setInt`, plus two semantic helpers used by
`RenderSystem`:
- `setPBRMaterial(albedo, metallic, roughness, ao)` -> sets `albedo`/`metallic`/`roughness`/`ao`.
- `setPBRLight(name, position, color, intensity)` -> sets `<name>.position/.color/.intensity`,
  e.g. `setPBRLight("lights[0]", ...)`.

**`resources/shaders/pbr.vert` + `pbr.frag`** is the active pipeline: a standard
metallic-roughness Cook-Torrance BRDF (GGX distribution, Smith geometry, Fresnel-Schlick),
up to `MAX_LIGHTS = 4` point lights, Reinhard tonemap + gamma correction. This is what
`RenderSystem` feeds.

**`resources/shaders/vertex.vert` + `fragment.frag`** is a corrected Blinn-Phong shader
(half-vector specular instead of the old reflect-vector Phong term, plus gamma
correction) kept as a working, loadable alternative — but nothing constructs a `Shader`
from these paths by default anymore, and `RenderSystem` doesn't know about its
`material.*`/`light.*` uniform names (those are a different naming scheme than the PBR
one). If you want a cheaper non-PBR path, you'd construct a second `Shader` from these
files and either add a per-entity "use basic shading" flag or a second render pass.

The PBR shader samples an optional `albedoMap` (`sampler2D`, gated by a `useAlbedoMap`
bool uniform): `RenderSystem` binds `MaterialComponent::albedoTexture` to texture unit 0
when present, otherwise falls back to the flat `albedo` color. `GRAPHICS::Texture`
(`src/Graphics/Texture.h`) either loads a file via `stb_image` (already available
system-wide as a dependency of `libimgui-dev`, see section 8) or generates a procedural
checkerboard (`Texture::createCheckerboard`) — the demo scene's player entity uses the
latter as a proof that UV sampling actually works end to end. Only an albedo/base-color
map exists so far — no normal/roughness/metallic/AO texture maps yet (see gap list).
The Blinn-Phong shader does not sample textures.

## 6. Physics & collision

- `RigidBodyComponent` is data only (see section 3); `PhysicsSystem` is the only thing
  that integrates it. There is no more `IPhysicsComponent`/`BasePhysicsComponent`
  polymorphic hierarchy — it added a layer of indirection and an abstract method
  (`getTotalForce`) that was never implemented, so nothing could actually be
  instantiated through it.
- `PHYSICS::BVH`/`BVHNode`/`AABB` (`src/Physics/BVH.h`) are unchanged algorithmically
  from before, just decoupled: they operate on `SpatialItem{entt::entity, position,
  radius}` instead of `shared_ptr<Actor>`. `BVH::build` also now bails out to a leaf
  instead of infinite-recursing if a split fails to separate any items (e.g. everything
  at the same x-coordinate) — the old code didn't guard against that.
- `CollisionSystem::resolveCollision` gives basic collision response: positional
  separation along the contact normal proportional to penetration depth, plus a
  restitution-scaled velocity reflection for whichever side(s) have a non-static
  `RigidBodyComponent` (this is also the first real reader of
  `RigidBodyComponent::restitution`, which used to be dead data — editable in the
  inspector but never consulted by anything). There's still no rotation/torque, no
  constraint/joint solver, and no static-geometry (ground/level mesh) collision — see
  gap list.
- `CollisionSystem` tracks stable entity pairs and publishes `"collision"`
  `CollisionEvent` values with Enter, Stay, and Exit phases through the injected
  `EventSystem`. Subscribe your own handler with
  `eventSystem->subscribe("collision", ...)` and hold onto the returned
  `SubscriptionId` so you can `unsubscribe` it when your subscriber is destroyed —
  `EventSystem` supports removing a single subscription instead of nuking every
  subscriber for that event type. `publish()` copies the handler list out before
  releasing its lock and invoking callbacks, so a handler is free to call
  `subscribe`/`unsubscribe`/`publish` on the same `EventSystem` (e.g. unsubscribing
  itself) without deadlocking on the internal mutex.

## 7. The editor (`src/Editor/EditorUI.h`)

A single class wrapping Dear ImGui: `beginFrame()`/`draw(registry)`/`endFrame()` called
once per frame from `Application::gameLoop` after all gameplay systems. Currently draws
two windows:
- **Scene** — every entity with a `TagComponent`, click to select.
- **Inspector** — for the selected entity, a `CollapsingHeader` per component it has
  (Transform, Material, RigidBody, Light, SphereCollider), with live-editable ImGui
  widgets bound directly to the component's fields.

To add a component to the inspector: add an `if (auto* c = registry.try_get<T>(selected))`
block in `EditorUI::drawInspector`. There's no reflection system — this is manual and
intentionally so, given the engine's current size.

## 8. Build system & third-party code

- **GLAD**: generated (not hand-written) via the `python3-glad` apt package for
  `gl:core=3.3`, output committed under `external/glad/`. Regenerate with:
  `python3 -m glad --api gl:core=3.3 --out-path external/glad --reproducible c`
  (only if you need a newer/different GL version — otherwise never touch this).
- **EnTT**: single header vendored at `external/entt/entt/entt.hpp`, pinned to v3.13.2.
  Update by re-downloading
  `https://raw.githubusercontent.com/skypjack/entt/<tag>/single_include/entt/entt.hpp`.
- **nlohmann/json**: single header vendored under `external/nlohmann/`, pinned to
  v3.11.3 with its MIT license. It backs the versioned SGE scene format.
- **Dear ImGui**: core (`imgui.h`/`imgui_internal.h` + prebuilt `libimgui.a`) comes from
  the system `libimgui-dev` package, found via `pkg-config imgui`. The GLFW/OpenGL3
  backend `.cpp`/`.h` files are **not** packaged as prebuilt — Debian ships their exact
  source under `/usr/share/doc/libimgui-dev/examples/backends/`, which we copied into
  `external/imgui_backends/`. **This means the vendored backend files must stay version
  matched to whatever `libimgui-dev` is installed** (currently 1.92.2b). If you bump the
  system package, re-copy those two files from the new doc package, or migrate to
  vendoring all of Dear ImGui from source instead of depending on the system package.
  The OpenGL3 backend is compiled with `IMGUI_IMPL_OPENGL_LOADER_CUSTOM` and a forced
  `-include glad/gl.h`, so it shares our GLAD-loaded function pointers instead of
  pulling in its own GL loader.
- Everything else (GLFW, GLM, OpenGL) is still found via the original CMake
  `find_package`/pkg-config fallback chain — unchanged.
- `resources/` is copied next to the built binary as a post-build step.
  `CORE::AssetLocator` discovers that executable-relative root and resolves validated
  relative asset paths from it, with a project working-directory fallback for
  development launches.

## 9. What's missing for an AA-scale game

Roughly in priority order — this list is the honest gap analysis, not a promise of
what will get built:

1. **Textures (partial).** `GRAPHICS::Texture` + `MaterialComponent::albedoTexture` +
   the `pbr.frag` `albedoMap` sampler now cover base-color textures (see section 5).
   Still missing: normal maps, roughness/metallic/AO maps (currently only flat scalar
   uniforms), texture atlasing, and any texture loaded from an on-disk asset in the
   default scene (the demo only uses the procedural checkerboard) — `Texture::loadFromFile`
   works but nothing in `buildScene()` calls it yet.
2. **Asset pipeline.** Models/shaders are selected by hardcoded asset paths at
   scene-construction time in `Application::buildScene()`. Path resolution is stable,
   but there's no asset registry,
   no hot-reload, no reference counting beyond `shared_ptr` (so identical models
   loaded twice become two separate GPU buffers unless you manually share the
   `shared_ptr`, as `buildScene()` currently does for the player/NPC cube).
3. **Collision response & a real physics story (partial).** `CollisionSystem::resolveCollision`
   now does basic sphere-vs-sphere positional separation + restitution-scaled velocity
   response (see section 6). Still missing: rotation/torque, constraints/joints,
   continuous collision detection (fast-moving objects can still tunnel through each
   other between frames), and static-geometry collision (there's no ground plane or
   level mesh in the demo scene at all, so nothing currently exercises
   `RigidBodyComponent.isStatic` beyond skipping integration for gravity). For anything
   beyond a tech demo you likely want to integrate a real physics library (Jolt, Bullet,
   PhysX) rather than continuing to hand-roll this.
4. **Scene authoring (partial).** Versioned JSON save/load, stable entity IDs,
   reference remapping, and Play/Stop serialization are implemented. The default
   scene is still hardcoded in `Application::buildScene()`, and the editor lacks
   Save/Open commands, migrations, prefabs, overrides, and autosave.
5. **Animation.** No skeletal mesh support, no animation blending/state machines — only
   static meshes.
6. **Audio.** Nothing exists — no audio backend, no sound component, no mixer.
7. **Shadow mapping / post-processing.** The old `Renderer` had unused
   `setupShadowRendering`/`enableShadows` scaffolding that was deleted along with the
   rest of that class since nothing called it. `Shader::setLightSpaceMatrix`/`setLightPos`
   are real (they now issue actual `glUniform` calls) but nothing constructs a shadow
   pass or a depth framebuffer. No bloom/tonemap-beyond-Reinhard/anti-aliasing/any other
   post-process pass either — `RenderSystem` draws straight to the default framebuffer.
8. **UI/HUD system distinct from the editor.** `EditorUI` is a debug tool, not a
   in-game UI toolkit — there's no HUD, menus, or dialog system for an actual player
   to see.
9. **Gameplay/scripting layer.** Everything is C++; there's no scripting language
   binding (Lua, etc.) for iteration speed, and no data-driven entity "prefab"/archetype
   system beyond manually calling `registry.emplace<T>` in C++.
10. **Multithreading / job system.** Systems run serially on the main thread; the
    engine has no job scheduler, so it won't scale to many entities or expensive
    systems without becoming frame-time bound.
11. **Testing & CI (partial).** `SGE_tests` now provides a small CTest regression
    suite for BVH, camera math, active-camera invariants, and registry cloning. A
    Linux GitHub Actions workflow builds and runs it. Coverage is still far below
    production needs and Windows/sanitizer lanes remain missing. `.devcontainer/`
    also still references the pre-GLAD dependency set and
    needs updating (add `python3-glad`, `libimgui-dev`; drop `libglew-dev`) if you want
    a working one-click dev environment.
12. **Windows/macOS verification.** This rewrite was built and verified on Linux/GCC
    only (in a sandboxed dev environment). The GLAD/CMake/ImGui-backend setup should
    port, but hasn't been exercised on MSVC or Apple Clang — the `MSVC`-specific
    branches in `CMakeLists.txt` (e.g. the `/FI` force-include for the ImGui OpenGL3
    backend) are written from documentation, not tested.
13. **Input rebinding / config.** Keybindings are hardcoded GLFW key constants in
    `Application::setup()` and `InputSystem`; no settings/config file, no rebinding UI.

None of this is meant to be discouraging — it's the actual distance between "a working
tech demo with a real architecture" (where the engine is now) and "AA game engine." The
ECS/systems foundation this rewrite put in place is what makes tackling this list
tractable one system at a time, rather than requiring another full rewrite.
