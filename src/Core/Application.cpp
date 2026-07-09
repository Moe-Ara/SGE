#include "Application.h"
#include "../Graphics/Window.h"
#include "../ECS/Components.h"
#include "../Systems/InputSystem.h"
#include "../Systems/PlayerControllerSystem.h"
#include "../Systems/FreeCameraControllerSystem.h"
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
#include <GLFW/glfw3.h>
#include <chrono>
#include <iostream>
#include <unordered_set>

namespace SGE::CORE {

    namespace {
        void errorCallback(int error, const char* description) {
            (void)error;
            std::cerr << "Error: " << description << std::endl;
        }

        template<typename Component>
        void copyComponent(entt::registry& src, entt::registry& dst) {
            auto view = src.view<Component>();
            for (auto entity : view) {
                dst.emplace_or_replace<Component>(entity, view.template get<Component>(entity));
            }
        }

        // entt::registry has no copy constructor (only move), so entering/exiting
        // Play mode needs an explicit deep copy of every entity and known
        // component type to snapshot/restore the pre-Play scene.
        entt::registry cloneRegistry(entt::registry& src) {
        entt::registry dst;

            std::unordered_set<entt::entity> entities;
            auto collect = [&entities](auto view) {
                for (auto entity : view) {
                    entities.insert(entity);
                }
            };
            collect(src.view<ECS::TransformComponent>());
            collect(src.view<ECS::MeshComponent>());
            collect(src.view<ECS::MaterialComponent>());
            collect(src.view<ECS::RigidBodyComponent>());
            collect(src.view<ECS::SphereColliderComponent>());
            collect(src.view<ECS::LightComponent>());
            collect(src.view<ECS::CameraComponent>());
            collect(src.view<ECS::FreeCameraComponent>());
            collect(src.view<ECS::ThirdPersonFollowComponent>());
            collect(src.view<ECS::PlayerControllerComponent>());
            collect(src.view<ECS::TagComponent>());

            // create() with a hint reuses that exact identifier on an empty
            // registry, so entity references between components (e.g. the
            // third-person camera's follow target) stay valid after the copy.
            for (auto entity : entities) {
                (void)dst.create(entity);
            }

            copyComponent<ECS::TransformComponent>(src, dst);
            copyComponent<ECS::MeshComponent>(src, dst);
            copyComponent<ECS::MaterialComponent>(src, dst);
            copyComponent<ECS::RigidBodyComponent>(src, dst);
            copyComponent<ECS::SphereColliderComponent>(src, dst);
            copyComponent<ECS::LightComponent>(src, dst);
            copyComponent<ECS::CameraComponent>(src, dst);
            copyComponent<ECS::FreeCameraComponent>(src, dst);
            copyComponent<ECS::ThirdPersonFollowComponent>(src, dst);
            copyComponent<ECS::PlayerControllerComponent>(src, dst);
            copyComponent<ECS::TagComponent>(src, dst);

            return dst;
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

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
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
            const auto& collisionEvent = static_cast<const SGE::EVENTS::CollisionEvent&>(event);
            (void)collisionEvent;
            std::cout << "Collision detected!" << std::endl;
        });

        auto shader = std::make_shared<SGE::GRAPHICS::Shader>("resources/shaders/pbr.vert", "resources/shaders/pbr.frag");
        auto skyboxShader = std::make_shared<SGE::GRAPHICS::Shader>("resources/shaders/skybox.vert", "resources/shaders/skybox.frag");

        // Bakes a procedural sky into a cubemap and convolves it into IBL data
        // (diffuse irradiance + prefiltered specular + BRDF LUT) once, up front.
        // This runs several offscreen render passes at small resolutions, which
        // leaves the GL viewport pointing at the wrong size afterward.
        auto environment = std::make_shared<SGE::GRAPHICS::Environment>();
        glViewport(0, 0, window.getWidth(), window.getHeight());

        // Order matters: controller systems modify ECS data, physics/collision
        // settle simulation state, then CameraRenderSystem prepares cameras from
        // transforms before the frame is rendered.
        simulationSystems.push_back(std::make_unique<SGE::SYSTEMS::InputSystem>(inputHandler));
        simulationSystems.push_back(std::make_unique<SGE::SYSTEMS::PlayerControllerSystem>(inputHandler));
        simulationSystems.push_back(std::make_unique<SGE::SYSTEMS::ThirdPersonCameraControllerSystem>(window, inputHandler));
        simulationSystems.push_back(std::make_unique<SGE::SYSTEMS::PhysicsSystem>());
        simulationSystems.push_back(std::make_unique<SGE::SYSTEMS::CollisionSystem>(eventSystem));

        presentationSystems.push_back(std::make_unique<SGE::SYSTEMS::FreeCameraControllerSystem>(window, inputHandler));
        presentationSystems.push_back(std::make_unique<SGE::SYSTEMS::CameraRenderSystem>(window));
        presentationSystems.push_back(std::make_unique<SGE::SYSTEMS::RenderSystem>(shader, environment));
        presentationSystems.push_back(std::make_unique<SGE::SYSTEMS::SkyboxSystem>(environment, skyboxShader));

        editorUI = std::make_unique<SGE::EDITOR::EditorUI>(window.getMWindow());

        buildScene();
    }

    void Application::buildScene() {
        std::shared_ptr<SGE::GAMEOBJECTS::Model> model =
            SGE::UTILS::ModelLoader::loadModelFromFile("resources/models/cube.obj");

        // Player: user-controlled, falls under gravity, collides.
        auto player = registry.create();
        registry.emplace<ECS::TagComponent>(player, "Player");
        registry.emplace<ECS::TransformComponent>(player);
        registry.emplace<ECS::MeshComponent>(player, model);
        auto& playerMaterial = registry.emplace<ECS::MaterialComponent>(player);
        playerMaterial.albedo = glm::vec3(0.8f, 0.1f, 0.1f);
        playerMaterial.metallic = 0.1f;
        playerMaterial.roughness = 0.4f;
        playerMaterial.albedoTexture = SGE::GRAPHICS::Texture::createCheckerboard(
            256, glm::vec3(0.8f, 0.1f, 0.1f), glm::vec3(0.9f, 0.9f, 0.9f));
        registry.emplace<ECS::RigidBodyComponent>(player);
        registry.emplace<ECS::SphereColliderComponent>(player);
        registry.emplace<ECS::PlayerControllerComponent>(player);

        // NPC: static prop sharing the same mesh.
        auto npc = registry.create();
        registry.emplace<ECS::TagComponent>(npc, "NPC");
        auto& npcTransform = registry.emplace<ECS::TransformComponent>(npc);
        npcTransform.translation = glm::vec3(10.0f, 0.0f, 2.0f);
        registry.emplace<ECS::MeshComponent>(npc, model);
        auto& npcMaterial = registry.emplace<ECS::MaterialComponent>(npc);
        npcMaterial.albedo = glm::vec3(0.85f, 0.85f, 0.85f);
        npcMaterial.metallic = 0.0f;
        npcMaterial.roughness = 0.6f;
        registry.emplace<ECS::SphereColliderComponent>(npc);

        // Camera: third-person follow of the player.
        auto cameraEntity = registry.create();
        registry.emplace<ECS::TagComponent>(cameraEntity, "MainCamera");
        auto& cameraTransform = registry.emplace<ECS::TransformComponent>(cameraEntity);
        cameraTransform.translation = glm::vec3(0.0f, 0.0f, 10.0f);
        cameraTransform.rotation = SGE::UTILS::lookRotation(-cameraTransform.translation);
        registry.emplace<ECS::CameraComponent>(cameraEntity);
        registry.emplace<ECS::FreeCameraComponent>(cameraEntity);
        auto& follow = registry.emplace<ECS::ThirdPersonFollowComponent>(cameraEntity);
        follow.target = player;
        follow.offset = glm::vec3(10.0f, 2.0f, 0.0f);
        follow.enabled = false;
        updateCameraControllerModes(false);

        // Light
        auto lightEntity = registry.create();
        registry.emplace<ECS::TagComponent>(lightEntity, "MainLight");
        auto& lightComp = registry.emplace<ECS::LightComponent>(lightEntity);
        lightComp.light.position = glm::vec3(5.0f, 10.0f, 5.0f);
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

        while (!window.closed()) {
            if (inputHandler->isKeyPressed(GLFW_KEY_ESCAPE)) {
                glfwSetWindowShouldClose(window.getMWindow(), GLFW_TRUE);
                break;
            }

            auto currentFrameTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> deltaTimeDuration = currentFrameTime - lastFrameTime;
            float deltaTime = deltaTimeDuration.count();
            lastFrameTime = currentFrameTime;

            window.clear();

            if (mode == EngineMode::Play) {
                for (auto& system : simulationSystems) {
                    system->update(registry, deltaTime);
                }
            }
            for (auto& system : presentationSystems) {
                system->update(registry, deltaTime);
            }

            editorUI->beginFrame();
            editorUI->draw(registry, mode);
            if (editorUI->consumePlayToggleRequest()) {
                togglePlayMode();
            }
            editorUI->endFrame();

            window.update();
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
        prePlaySnapshot = cloneRegistry(registry);
        updateCameraControllerModes(true);
        mode = EngineMode::Play;
    }

    void Application::exitPlayMode() {
        registry = std::move(prePlaySnapshot);
        updateCameraControllerModes(false);
        mode = EngineMode::Inspection;
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
