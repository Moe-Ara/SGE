#include "Application.h"
#include "../Graphics/Window.h"
#include "../ECS/Components.h"
#include "../ECS/RegistryClone.h"
#include "../ECS/Queries.h"
#include "../Systems/InputSystem.h"
#include "../Systems/PlayerControllerSystem.h"
#include "../Systems/FreeCameraControllerSystem.h"
#include "../Systems/LuaScriptSystem.h"
#include "../Systems/ThirdPersonCameraControllerSystem.h"
#include "../Systems/CameraRenderSystem.h"
#include "../Systems/PhysicsSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/SkyboxSystem.h"
#include "../Graphics/Shader.h"
#include "../Graphics/Texture.h"
#include "../Graphics/Environment.h"
#include "../Utils/ModelLoader.h"
#include "../Utils/CameraMath.h"
#include "../Events/CollisionEvent.h"
#include "AssetLocator.h"
#include <GLFW/glfw3.h>
#include <chrono>
#include <algorithm>
#include <iostream>

namespace SGE::CORE {

    namespace {
        void errorCallback(int error, const char* description) {
            (void)error;
            std::cerr << "Error: " << description << std::endl;
        }

        void setCameraControllerModes(entt::registry& registry, bool playMode) {
            auto freeView = registry.view<ECS::FreeCameraComponent>();
            for (auto entity : freeView) {
                auto& freeCam = freeView.get<ECS::FreeCameraComponent>(entity);
                freeCam.enabled = !playMode;
            }

            auto followView = registry.view<ECS::ThirdPersonFollowComponent>();
            for (auto entity : followView) {
                auto& follow = followView.get<ECS::ThirdPersonFollowComponent>(entity);
                follow.enabled = playMode;
            }
        }
    }

    Application::Application() {
        setup();
    }

    Application::~Application() {
        cleanup();
    }

    void Application::setup() {
        glfwSetErrorCallback(errorCallback);

        auto& window = SGE::GRAPHICS::Window::getInstance("SGE", 960, 540);
        if (window.isHeadless()) {
            std::cout << "Headless environment detected; skipping scene/editor setup." << std::endl;
            return;
        }

        std::cerr << "[SGE] Window and OpenGL context ready.\n";

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        glDisable(GL_BLEND);
        glClearColor(0.05f, 0.07f, 0.09f, 1.0f);

        SGE::INPUT::InputHandler::setupKeyHandler(window);
        const std::vector<int> keysToMonitor = {
            GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_W, GLFW_KEY_D, GLFW_KEY_LEFT_CONTROL,
            GLFW_KEY_LEFT_SHIFT, GLFW_KEY_Q, GLFW_KEY_E, GLFW_KEY_SPACE, GLFW_KEY_ESCAPE
        };
        const std::vector<int> buttonsToMonitor = {GLFW_MOUSE_BUTTON_RIGHT, GLFW_MOUSE_BUTTON_LEFT};
        inputHandler = std::make_shared<SGE::INPUT::InputHandler>(keysToMonitor, buttonsToMonitor);

        eventSystem = std::make_shared<SGE::EVENTS::EventSystem>();
        collisionSubscription = eventSystem->subscribe("collision", [](const SGE::EVENTS::IEvent& event) {
            if (const auto* collisionEvent = dynamic_cast<const SGE::EVENTS::CollisionEvent*>(&event)) {
                (void)collisionEvent;
            }
        });

        std::cerr << "[SGE] Loading render shaders...\n";
        auto shader = std::make_shared<SGE::GRAPHICS::Shader>(
            AssetLocator::resolve("shaders/pbr.vert").string(),
            AssetLocator::resolve("shaders/pbr.frag").string()
        );
        auto skyboxShader = std::make_shared<SGE::GRAPHICS::Shader>(
            AssetLocator::resolve("shaders/skybox.vert").string(),
            AssetLocator::resolve("shaders/skybox.frag").string()
        );

        // Bakes a procedural sky into a cubemap and convolves it into IBL data
        // (diffuse irradiance + prefiltered specular + BRDF LUT) once, up front.
        // This runs several offscreen render passes at small resolutions, which
        // leaves the GL viewport pointing at the wrong size afterward.
        std::cerr << "[SGE] Baking environment lighting...\n";
        auto environment = std::make_shared<SGE::GRAPHICS::Environment>();
        glViewport(0, 0, window.getFramebufferWidth(), window.getFramebufferHeight());

        // Order matters: controller systems modify ECS data, physics/collision
        // settle simulation state, then CameraRenderSystem prepares cameras from
        // transforms before the frame is rendered.
        simulationSystems.emplace<SGE::SYSTEMS::InputSystem>(inputHandler);
        simulationSystems.emplace<SGE::SYSTEMS::PlayerControllerSystem>(inputHandler);
        simulationSystems.emplace<SGE::SYSTEMS::ThirdPersonCameraControllerSystem>(window, inputHandler);
        simulationSystems.emplace<SGE::SYSTEMS::LuaScriptSystem>();
        simulationSystems.emplace<SGE::SYSTEMS::PhysicsSystem>();
        simulationSystems.emplace<SGE::SYSTEMS::CollisionSystem>(eventSystem);

        presentationSystems.emplace<SGE::SYSTEMS::FreeCameraControllerSystem>(window, inputHandler);
        presentationSystems.emplace<SGE::SYSTEMS::CameraRenderSystem>(window);
        presentationSystems.emplace<SGE::SYSTEMS::RenderSystem>(shader, environment);
        presentationSystems.emplace<SGE::SYSTEMS::SkyboxSystem>(environment, skyboxShader);

        editorUI = std::make_unique<SGE::EDITOR::EditorUI>(window.getMWindow());

        buildScene();
        std::cerr << "[SGE] Scene and editor ready.\n";
    }

    void Application::buildScene() {
        std::shared_ptr<SGE::GAMEOBJECTS::Model> model =
            SGE::UTILS::ModelLoader::loadModelFromFile(
                AssetLocator::resolve("models/cube.obj").string()
            );

        // Player: user-controlled, falls under gravity, collides.
        auto player = registry.create();
        registry.emplace<ECS::SceneIdentityComponent>(player, 1u);
        registry.emplace<ECS::TagComponent>(player, "Player");
        registry.emplace<ECS::TransformComponent>(player);
        registry.emplace<ECS::MeshComponent>(player, model, "models/cube.obj");
        auto& playerMaterial = registry.emplace<ECS::MaterialComponent>(player);
        playerMaterial.albedo = glm::vec3(0.8f, 0.1f, 0.1f);
        playerMaterial.metallic = 0.1f;
        playerMaterial.roughness = 0.4f;
        playerMaterial.albedoTexture = SGE::GRAPHICS::Texture::createCheckerboard(
            256, glm::vec3(0.8f, 0.1f, 0.1f), glm::vec3(0.9f, 0.9f, 0.9f));
        playerMaterial.albedoTextureAssetId = "generated:player-checkerboard";
        registry.emplace<ECS::RigidBodyComponent>(player);
        registry.emplace<ECS::SphereColliderComponent>(player);
        registry.emplace<ECS::PlayerControllerComponent>(player);

        // NPC: static prop sharing the same mesh.
        auto npc = registry.create();
        registry.emplace<ECS::SceneIdentityComponent>(npc, 2u);
        registry.emplace<ECS::TagComponent>(npc, "NPC");
        auto& npcTransform = registry.emplace<ECS::TransformComponent>(npc);
        npcTransform.translation = glm::vec3(10.0f, 0.0f, 2.0f);
        registry.emplace<ECS::MeshComponent>(npc, model, "models/cube.obj");
        auto& npcMaterial = registry.emplace<ECS::MaterialComponent>(npc);
        npcMaterial.albedo = glm::vec3(0.85f, 0.85f, 0.85f);
        npcMaterial.metallic = 0.0f;
        npcMaterial.roughness = 0.6f;
        registry.emplace<ECS::SphereColliderComponent>(npc);
        registry.emplace<ECS::ScriptComponent>(npc, "scripts/example.lua", true);

        // Camera: third-person follow of the player.
        auto cameraEntity = registry.create();
        registry.emplace<ECS::SceneIdentityComponent>(cameraEntity, 3u);
        registry.emplace<ECS::TagComponent>(cameraEntity, "MainCamera");
        auto& cameraTransform = registry.emplace<ECS::TransformComponent>(cameraEntity);
        cameraTransform.translation = glm::vec3(0.0f, 0.0f, 10.0f);
        cameraTransform.rotation = SGE::UTILS::lookRotation(-cameraTransform.translation);
        registry.emplace<ECS::CameraComponent>(cameraEntity);
        ECS::setPrimaryCamera(registry, cameraEntity);
        registry.emplace<ECS::FreeCameraComponent>(cameraEntity);
        auto& follow = registry.emplace<ECS::ThirdPersonFollowComponent>(cameraEntity);
        follow.target = player;
        follow.offset = glm::vec3(10.0f, 2.0f, 0.0f);
        follow.enabled = false;
        updateCameraControllerModes(false);

        // Light
        auto lightEntity = registry.create();
        registry.emplace<ECS::SceneIdentityComponent>(lightEntity, 4u);
        registry.emplace<ECS::TagComponent>(lightEntity, "MainLight");
        auto& lightTransform = registry.emplace<ECS::TransformComponent>(lightEntity);
        lightTransform.translation = glm::vec3(5.0f, 10.0f, 5.0f);
        auto& lightComp = registry.emplace<ECS::LightComponent>(lightEntity);
        lightComp.light.color = glm::vec3(1.0f, 1.0f, 0.95f);
        lightComp.light.intensity = 300.0f;
    }

    void Application::run() {
        auto& window = SGE::GRAPHICS::Window::getInstance();
        if (window.isHeadless()) {
            std::cout << "Headless mode: skipping game loop." << std::endl;
            return;
        }

        gameLoop();
    }

    void Application::gameLoop() {
        auto& window = SGE::GRAPHICS::Window::getInstance();
        auto lastFrameTime = std::chrono::high_resolution_clock::now();
        constexpr float fixedDeltaTime = 1.0f / 60.0f;
        constexpr float maximumFrameTime = 0.25f;
        constexpr int maximumCatchUpSteps = 8;
        float simulationAccumulator = 0.0f;

        while (!window.closed()) {
            window.pollEvents();
            if (glfwGetKey(window.getMWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetWindowShouldClose(window.getMWindow(), GLFW_TRUE);
                break;
            }

            auto currentFrameTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> deltaTimeDuration = currentFrameTime - lastFrameTime;
            const float deltaTime = std::clamp(deltaTimeDuration.count(), 0.0f, maximumFrameTime);
            lastFrameTime = currentFrameTime;

            window.clear();
            editorUI->beginFrame();
            inputHandler->setKeyboardCaptured(editorUI->wantsKeyboardCapture());
            inputHandler->setMouseCaptured(editorUI->wantsMouseCapture());
            bool inputEdgesConsumed = mode != EngineMode::Play;

            if (mode == EngineMode::Play) {
                simulationAccumulator += deltaTime;
                int steps = 0;
                while (simulationAccumulator >= fixedDeltaTime && steps < maximumCatchUpSteps) {
                    simulationSystems.update(registry, fixedDeltaTime);
                    simulationAccumulator -= fixedDeltaTime;
                    ++steps;
                    inputEdgesConsumed = true;
                }
                if (steps == maximumCatchUpSteps) {
                    simulationAccumulator = 0.0f;
                }
            } else {
                simulationAccumulator = 0.0f;
            }
            presentationSystems.update(registry, deltaTime);

            editorUI->draw(registry, mode);
            if (editorUI->consumePlayToggleRequest()) {
                togglePlayMode();
            }
            editorUI->endFrame();

            if (inputEdgesConsumed) {
                inputHandler->endFrame();
            }
            window.present();
        }
    }

    void Application::togglePlayMode() {
        if (mode == EngineMode::Inspection) {
            enterPlayMode();
        } else {
            exitPlayMode();
        }
    }

    void Application::enterPlayMode() {
        prePlaySnapshot = ECS::cloneRegistry(registry);
        updateCameraControllerModes(true);
        mode = EngineMode::Play;
        std::cerr << "[SGE] Entered Play mode.\n";
    }

    void Application::exitPlayMode() {
        const std::uint64_t selectedSceneId = editorUI->selectedSceneId(registry);
        registry = std::move(prePlaySnapshot);
        simulationSystems.onWorldReset();
        presentationSystems.onWorldReset();
        editorUI->restoreSelection(registry, selectedSceneId);
        updateCameraControllerModes(false);
        mode = EngineMode::Inspection;
        std::cerr << "[SGE] Returned to Inspection mode.\n";
    }

    void Application::updateCameraControllerModes(bool playMode) {
        setCameraControllerModes(registry, playMode);
    }

    void Application::cleanup() {
        if (eventSystem) {
            eventSystem->unsubscribe("collision", collisionSubscription);
        }
    }
}
