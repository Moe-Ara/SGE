#include "src/Physics/BVH.h"
#include "src/Graphics/Camera.h"
#include "src/ECS/RegistryClone.h"
#include "src/ECS/Queries.h"
#include "src/Utils/CameraMath.h"
#include "src/Core/ObjectPool.h"
#include "src/ECS/SystemPipeline.h"
#include "src/Events/EventSystem.h"
#include "src/Events/CollisionEvent.h"
#include "src/Systems/CollisionSystem.h"
#include "src/Systems/LuaScriptSystem.h"
#include "src/Core/AssetLocator.h"
#include "src/ECS/SceneSerializer.h"
#include <filesystem>
#include <fstream>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {

    int failures = 0;

    void expect(bool condition, const char* message) {
        if (!condition) {
            std::cerr << "FAILED: " << message << '\n';
            ++failures;
        }
    }

    void testCrossPartitionSphereCollision() {
        using namespace SGE::PHYSICS;
        const std::vector<SpatialItem> items{
            {entt::entity{0}, {-100.0f, 0.0f, 0.0f}, 1.0f},
            {entt::entity{1}, {-50.0f, 0.0f, 0.0f}, 1.0f},
            {entt::entity{2}, {-1.0f, 0.0f, 0.0f}, 2.0f},
            {entt::entity{3}, {1.0f, 0.0f, 0.0f}, 2.0f},
            {entt::entity{4}, {50.0f, 0.0f, 0.0f}, 1.0f},
            {entt::entity{5}, {100.0f, 0.0f, 0.0f}, 1.0f}
        };

        BVH bvh;
        bvh.build(items);
        std::vector<CollisionInfo> collisions;
        bvh.getCollisions(collisions);

        expect(collisions.size() == 1,
               "BVH must detect an overlap crossing an internal partition");
        if (!collisions.empty()) {
            expect(std::abs(collisions.front().contactPoint.x) < 0.001f,
                   "equal sphere contact point must lie midway between witness points");
        }
    }

    void testBVHQueryFiltersLeafItems() {
        using namespace SGE::PHYSICS;
        BVH bvh;
        bvh.build({
            {entt::entity{0}, {0.0f, 0.0f, 0.0f}, 1.0f},
            {entt::entity{1}, {10.0f, 0.0f, 0.0f}, 1.0f}
        });

        std::vector<SpatialItem> result;
        bvh.query(AABB{{-2.0f, -2.0f, -2.0f}, {2.0f, 2.0f, 2.0f}}, result);
        expect(result.size() == 1 && result.front().entity == entt::entity{0},
               "BVH range query must filter individual items in a leaf");
    }

    void testYawPitchRoundTrip() {
        const glm::vec3 forward = SGE::UTILS::forwardFromYawPitch(37.0f, -24.0f);
        const glm::vec2 yawPitch = SGE::UTILS::yawPitchFromForward(forward);
        expect(std::abs(yawPitch.x - 37.0f) < 0.001f,
               "camera yaw must round-trip through forward vector");
        expect(std::abs(yawPitch.y + 24.0f) < 0.001f,
               "camera pitch must round-trip through forward vector");
    }

    void testCameraRejectsInvalidPerspective() {
        SGE::GRAPHICS::Camera camera;
        bool threw = false;
        try {
            camera.setPerspectiveProjection(glm::radians(60.0f), 0.0f, 0.1f, 100.0f);
        } catch (const std::invalid_argument&) {
            threw = true;
        }
        expect(threw, "camera must reject a zero perspective aspect ratio");
    }

    void testCameraHandlesDegenerateDirection() {
        SGE::GRAPHICS::Camera camera;
        camera.setViewDirection({1.0f, 2.0f, 3.0f}, {0.0f, 0.0f, 0.0f});
        const glm::mat4 view = camera.getViewMatrix();
        bool finite = true;
        for (int column = 0; column < 4; ++column) {
            for (int row = 0; row < 4; ++row) {
                finite = finite && std::isfinite(view[column][row]);
            }
        }
        expect(finite, "degenerate camera direction must not produce NaNs");
    }

    void testRegistryClonePreservesOrphansAndRejectsUnknownComponents() {
        struct UnknownComponent { int value; };

        entt::registry source;
        const entt::entity orphan = source.create();
        const entt::entity tagged = source.create();
        source.emplace<SGE::ECS::TagComponent>(tagged, "Tagged");

        entt::registry clone = SGE::ECS::cloneRegistry(source);
        const std::uint64_t orphanId = source.get<SGE::ECS::SceneIdentityComponent>(orphan).id;
        const std::uint64_t taggedId = source.get<SGE::ECS::SceneIdentityComponent>(tagged).id;
        entt::entity clonedOrphan = entt::null;
        entt::entity clonedTagged = entt::null;
        for (const auto entity : clone.view<SGE::ECS::SceneIdentityComponent>()) {
            const auto id = clone.get<SGE::ECS::SceneIdentityComponent>(entity).id;
            if (id == orphanId) clonedOrphan = entity;
            if (id == taggedId) clonedTagged = entity;
        }
        expect(clonedOrphan != entt::null, "registry clone must preserve orphan entities");
        expect(clonedTagged != entt::null &&
                   clone.get<SGE::ECS::TagComponent>(clonedTagged).name == "Tagged",
               "registry clone must preserve registered component values");

        source.emplace<UnknownComponent>(tagged, 42);
        bool threw = false;
        try {
            (void)SGE::ECS::cloneRegistry(source);
        } catch (const std::runtime_error&) {
            threw = true;
        }
        expect(threw, "registry clone must reject unregistered components instead of losing them");
    }

    void testPrimaryCameraInvariant() {
        entt::registry registry;
        const entt::entity first = registry.create();
        const entt::entity second = registry.create();
        registry.emplace<SGE::ECS::CameraComponent>(first);
        registry.emplace<SGE::ECS::CameraComponent>(second);
        SGE::ECS::setPrimaryCamera(registry, second);
        expect(SGE::ECS::findPrimaryCamera(registry) == second,
               "setPrimaryCamera must establish one active camera");

        registry.get<SGE::ECS::CameraComponent>(first).isPrimary = true;
        bool threw = false;
        try {
            (void)SGE::ECS::findPrimaryCamera(registry);
        } catch (const std::logic_error&) {
            threw = true;
        }
        expect(threw, "multiple primary cameras must fail loudly");
    }

    void testObjectPoolReusesStableObjects() {
        struct PooledValue { int value{0}; };
        SGE::CORE::ObjectPool<PooledValue> pool;
        PooledValue* first = &pool.acquire();
        PooledValue* second = &pool.acquire();
        first->value = 7;
        pool.reset();

        expect(&pool.acquire() == first, "object pool must reuse its first object after reset");
        expect(&pool.acquire() == second, "object pool must preserve stable pooled addresses");
        expect(pool.capacity() == 2, "object pool must not allocate while reusing its high watermark");
    }

    void testSystemPipelineRunsInInsertionOrder() {
        struct CountingSystem final : SGE::ECS::ISystem {
            CountingSystem(std::vector<int>& output, int value) : output(output), value(value) {}
            void update(entt::registry&, float) override { output.push_back(value); }
            void onWorldReset() override { output.push_back(-value); }
            std::vector<int>& output;
            int value;
        };

        std::vector<int> order;
        SGE::ECS::SystemPipeline pipeline;
        pipeline.emplace<CountingSystem>(order, 1);
        pipeline.emplace<CountingSystem>(order, 2);
        entt::registry registry;
        pipeline.update(registry, 1.0f / 60.0f);
        expect(order == std::vector<int>({1, 2}),
               "system pipeline must execute systems in insertion order");
        pipeline.onWorldReset();
        expect(order == std::vector<int>({1, 2, -1, -2}),
               "system pipeline must notify stateful systems when the world is replaced");
    }

    void testBVHNodePoolStopsGrowingForStableWorkload() {
        using namespace SGE::PHYSICS;
        std::vector<SpatialItem> items;
        for (int i = 0; i < 32; ++i) {
            items.push_back({static_cast<entt::entity>(i), {static_cast<float>(i), 0.0f, 0.0f}, 0.4f});
        }
        BVH bvh;
        bvh.build(items);
        const std::size_t highWatermark = bvh.allocatedNodeCount();
        bvh.build(items);
        expect(bvh.allocatedNodeCount() == highWatermark,
               "BVH node pool must not grow for an unchanged workload");
        expect(bvh.activeNodeCount() <= bvh.allocatedNodeCount(),
               "BVH active node count must stay inside pool capacity");
    }

    void testCollisionContactLifecycle() {
        using SGE::EVENTS::CollisionEvent;
        using SGE::EVENTS::CollisionPhase;

        auto events = std::make_shared<SGE::EVENTS::EventSystem>();
        std::vector<CollisionPhase> phases;
        std::vector<std::pair<entt::entity, entt::entity>> pairs;
        events->subscribe("collision", [&phases, &pairs](const SGE::EVENTS::IEvent& event) {
            const auto* collision = dynamic_cast<const CollisionEvent*>(&event);
            if (collision) {
                phases.push_back(collision->getPhase());
                pairs.emplace_back(collision->getEntityA(), collision->getEntityB());
            }
        });

        entt::registry registry;
        const entt::entity a = registry.create();
        const entt::entity b = registry.create();
        registry.emplace<SGE::ECS::TransformComponent>(a).translation = {0.0f, 0.0f, 0.0f};
        registry.emplace<SGE::ECS::TransformComponent>(b).translation = {1.0f, 0.0f, 0.0f};
        registry.emplace<SGE::ECS::SphereColliderComponent>(a, 1.0f);
        registry.emplace<SGE::ECS::SphereColliderComponent>(b, 1.0f);

        SGE::SYSTEMS::CollisionSystem collisions{events};
        collisions.update(registry, 1.0f / 60.0f);
        collisions.update(registry, 1.0f / 60.0f);
        registry.get<SGE::ECS::TransformComponent>(b).translation.x = 5.0f;
        collisions.update(registry, 1.0f / 60.0f);

        expect(phases == std::vector<CollisionPhase>({
                   CollisionPhase::Enter,
                   CollisionPhase::Stay,
                   CollisionPhase::Exit
               }),
               "collision contacts must emit enter, stay, and exit in order");
        expect(pairs.size() == 3 && pairs[0] == pairs[1] && pairs[1] == pairs[2],
               "collision event entity order must remain stable for the contact lifetime");
    }

    void testAssetLocatorUsesConfiguredRoot() {
        const std::filesystem::path root =
            std::filesystem::temp_directory_path() / "sge_asset_locator_test";
        std::filesystem::remove_all(root);
        std::filesystem::create_directories(root / "shaders");
        {
            std::ofstream file(root / "shaders" / "test.vert");
            file << "test";
        }

        SGE::CORE::AssetLocator::initializeFromRoot(root);
        expect(SGE::CORE::AssetLocator::resolve("shaders/test.vert") ==
                   std::filesystem::weakly_canonical(root / "shaders" / "test.vert"),
               "asset locator must resolve files from its configured root");

        bool traversalRejected = false;
        try {
            (void)SGE::CORE::AssetLocator::resolve("../outside.txt");
        } catch (const std::invalid_argument&) {
            traversalRejected = true;
        }
        expect(traversalRejected, "asset locator must reject parent traversal");

        bool missingRejected = false;
        try {
            (void)SGE::CORE::AssetLocator::resolve("models/missing.obj");
        } catch (const std::runtime_error&) {
            missingRejected = true;
        }
        expect(missingRejected, "asset locator must report missing assets");
        std::filesystem::remove_all(root);
    }

    void testLuaScriptLifecycleMutatesTransform() {
        const auto root = std::filesystem::temp_directory_path() / "sge_lua_test";
        std::filesystem::remove_all(root);
        std::filesystem::create_directories(root / "scripts");
        {
            std::ofstream script(root / "scripts" / "motion.lua");
            script << "function start() entity:set_position(1, 2, 3) end\n"
                      "function update(dt) entity:translate(dt, 0, 0) end\n";
        }

        SGE::CORE::AssetLocator::initializeFromRoot(root);
        entt::registry registry;
        const auto entity = registry.create();
        registry.emplace<SGE::ECS::TransformComponent>(entity);
        registry.emplace<SGE::ECS::ScriptComponent>(entity, "scripts/motion.lua", true);

        SGE::SYSTEMS::LuaScriptSystem scripts;
        scripts.update(registry, 0.5f);
        const auto& position =
            registry.get<SGE::ECS::TransformComponent>(entity).translation;
        expect(glm::length(position - glm::vec3{1.5f, 2.0f, 3.0f}) < 0.0001f,
               "Lua start and update callbacks must mutate the owning transform");
        std::filesystem::remove_all(root);
    }

    entt::entity findSceneEntity(entt::registry& registry, std::uint64_t id) {
        for (const auto entity : registry.view<SGE::ECS::SceneIdentityComponent>()) {
            if (registry.get<SGE::ECS::SceneIdentityComponent>(entity).id == id) {
                return entity;
            }
        }
        return entt::null;
    }

    void testFullSceneSerializationRoundTrip() {
        using namespace SGE::ECS;
        auto model = std::shared_ptr<SGE::GAMEOBJECTS::Model>(
            reinterpret_cast<SGE::GAMEOBJECTS::Model*>(0x1), [](auto*) {}
        );
        auto texture = std::shared_ptr<SGE::GRAPHICS::Texture>(
            reinterpret_cast<SGE::GRAPHICS::Texture*>(0x1), [](auto*) {}
        );

        entt::registry source;
        const entt::entity target = source.create();
        source.emplace<SceneIdentityComponent>(target, 101u);
        source.emplace<TagComponent>(target, "Target");
        source.emplace<TransformComponent>(target).translation = {3.0f, 4.0f, 5.0f};
        source.emplace<MeshComponent>(target, model, "models/test.glb");
        auto& material = source.emplace<MaterialComponent>(target);
        material.albedo = {0.2f, 0.4f, 0.6f};
        material.metallic = 0.7f;
        material.roughness = 0.3f;
        material.ao = 0.8f;
        material.albedoTexture = texture;
        material.albedoTextureAssetId = "textures/test.png";
        auto& body = source.emplace<RigidBodyComponent>(target);
        body.velocity = {1.0f, 2.0f, 3.0f};
        body.mass = 2.5f;
        body.restitution = 0.25f;
        source.emplace<SphereColliderComponent>(target, 1.25f);
        auto& light = source.emplace<LightComponent>(target).light;
        light.color = {0.9f, 0.8f, 0.7f};
        light.intensity = 42.0f;
        source.emplace<PlayerControllerComponent>(target, 6.0f, 9.0f, true);
        source.emplace<ScriptComponent>(target, "scripts/test.lua", true);

        const entt::entity cameraEntity = source.create();
        source.emplace<SceneIdentityComponent>(cameraEntity, 202u);
        source.emplace<TagComponent>(cameraEntity, "Camera");
        source.emplace<TransformComponent>(cameraEntity);
        auto& camera = source.emplace<CameraComponent>(cameraEntity);
        camera.isPrimary = true;
        camera.fovYRadians = glm::radians(65.0f);
        source.emplace<FreeCameraComponent>(cameraEntity).movementSpeed = 8.0f;
        auto& follow = source.emplace<ThirdPersonFollowComponent>(cameraEntity);
        follow.target = target;
        follow.offset = {2.0f, 3.0f, 4.0f};

        entt::registry clone = SceneSerializer::cloneRuntime(source);
        const entt::entity clonedTarget = findSceneEntity(clone, 101u);
        const entt::entity clonedCamera = findSceneEntity(clone, 202u);
        expect(clonedTarget != entt::null && clonedCamera != entt::null,
               "scene round-trip must preserve stable entity IDs");
        expect(clone.get<TagComponent>(clonedTarget).name == "Target" &&
                   clone.get<TransformComponent>(clonedTarget).translation == glm::vec3(3.0f, 4.0f, 5.0f),
               "scene round-trip must preserve tag and transform data");
        expect(clone.get<ScriptComponent>(clonedTarget).assetId == "scripts/test.lua" &&
                   clone.get<ScriptComponent>(clonedTarget).enabled,
               "scene round-trip must preserve Lua script configuration");
        expect(clone.get<MeshComponent>(clonedTarget).model == model &&
                   clone.get<MaterialComponent>(clonedTarget).albedoTexture == texture,
               "runtime scene clone must resolve model and texture asset IDs");
        expect(clone.get<RigidBodyComponent>(clonedTarget).mass == 2.5f &&
                   clone.get<SphereColliderComponent>(clonedTarget).radius == 1.25f &&
                   clone.get<LightComponent>(clonedTarget).light.intensity == 42.0f,
               "scene round-trip must preserve physics and light data");
        expect(clone.get<ThirdPersonFollowComponent>(clonedCamera).target == clonedTarget,
               "scene round-trip must remap entity references to loaded handles");
        expect(clone.get<CameraComponent>(clonedCamera).isPrimary &&
                   clone.get<FreeCameraComponent>(clonedCamera).movementSpeed == 8.0f,
               "scene round-trip must preserve camera settings");

        const std::filesystem::path scenePath =
            std::filesystem::temp_directory_path() / "sge_scene_round_trip.json";
        SceneSerializer::save(source, scenePath);
        SceneSerializer::save(source, scenePath);
        SceneAssetResolver resolver;
        resolver.model = [model](const std::string& id) {
            return id == "models/test.glb" ? model : std::shared_ptr<SGE::GAMEOBJECTS::Model>{};
        };
        resolver.texture = [texture](const std::string& id) {
            return id == "textures/test.png" ? texture : std::shared_ptr<SGE::GRAPHICS::Texture>{};
        };
        entt::registry loaded = SceneSerializer::load(scenePath, resolver);
        const entt::entity loadedTarget = findSceneEntity(loaded, 101u);
        expect(loadedTarget != entt::null && loaded.get<MeshComponent>(loadedTarget).model == model &&
                   loaded.get<MaterialComponent>(loadedTarget).albedoTexture == texture,
               "scene files must load and resolve durable asset IDs");
        std::filesystem::remove(scenePath);
    }

    void testSceneSerializerRejectsInvalidDocuments() {
        using SGE::ECS::SceneSerializer;
        const std::vector<std::string> invalidDocuments{
            R"({"format":"SGE.Scene","version":999,"entities":[]})",
            R"({"format":"SGE.Scene","version":1,"entities":[{"id":1,"components":{"Unknown":{}}}]})",
            R"({"format":"SGE.Scene","version":1,"entities":[{"id":1,"components":{}},{"id":1,"components":{}}]})",
            R"({"format":"SGE.Scene","version":1,"entities":[{"id":1,"components":{"ThirdPersonFollow":{"target":2,"offset":[0,0,0],"yaw":0,"pitch":0,"mouseSensitivity":1,"enabled":true}}}]})"
        };
        for (const auto& document : invalidDocuments) {
            bool threw = false;
            try {
                (void)SceneSerializer::deserialize(document);
            } catch (const std::runtime_error&) {
                threw = true;
            }
            expect(threw, "scene serializer must reject unsupported or malformed documents");
        }
    }

} // namespace

int main() {
    testCrossPartitionSphereCollision();
    testBVHQueryFiltersLeafItems();
    testYawPitchRoundTrip();
    testCameraRejectsInvalidPerspective();
    testCameraHandlesDegenerateDirection();
    testRegistryClonePreservesOrphansAndRejectsUnknownComponents();
    testPrimaryCameraInvariant();
    testObjectPoolReusesStableObjects();
    testSystemPipelineRunsInInsertionOrder();
    testBVHNodePoolStopsGrowingForStableWorkload();
    testCollisionContactLifecycle();
    testAssetLocatorUsesConfiguredRoot();
    testLuaScriptLifecycleMutatesTransform();
    testFullSceneSerializationRoundTrip();
    testSceneSerializerRejectsInvalidDocuments();

    if (failures == 0) {
        std::cout << "All engine regression tests passed\n";
    }
    return failures == 0 ? 0 : 1;
}
