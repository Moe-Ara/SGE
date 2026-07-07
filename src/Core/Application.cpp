#include "Application.h"
#include "../Graphics/Window.h"
#include "../ECS/Components.h"
#include "../Systems/InputSystem.h"
#include "../Systems/CameraSystem.h"
#include "../Systems/PhysicsSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/SkyboxSystem.h"
#include "../Graphics/Shader.h"
#include "../Graphics/Texture.h"
#include "../Graphics/Environment.h"
#include "../Utils/ModelLoader.h"
#include "../Events/CollisionEvent.h"
#include <GLFW/glfw3.h>
#include <chrono>
#include <iostream>

namespace SGE::CORE {

    namespace {
        void errorCallback(int error, const char* description) {
            (void)error;
            std::cerr << "Error: " << description << std::endl;
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

        // Order matters: input sets desired velocity/movement, physics integrates
        // it into position, collision resolves overlaps against the final
        // position, then the camera follows and the frame is rendered - each
        // stage sees the previous stage's finished result, not last frame's.
        // The skybox is drawn last so it only fills in pixels nothing else drew.
        systems.push_back(std::make_unique<SGE::SYSTEMS::InputSystem>(inputHandler));
        systems.push_back(std::make_unique<SGE::SYSTEMS::PhysicsSystem>());
        systems.push_back(std::make_unique<SGE::SYSTEMS::CollisionSystem>(eventSystem));
        systems.push_back(std::make_unique<SGE::SYSTEMS::CameraSystem>(window));
        systems.push_back(std::make_unique<SGE::SYSTEMS::RenderSystem>(shader, environment));
        systems.push_back(std::make_unique<SGE::SYSTEMS::SkyboxSystem>(environment, skyboxShader));

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
        registry.emplace<ECS::CameraComponent>(cameraEntity);
        auto& follow = registry.emplace<ECS::ThirdPersonFollowComponent>(cameraEntity);
        follow.target = player;
        follow.offset = glm::vec3(10.0f, 2.0f, 0.0f);

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

            for (auto& system : systems) {
                system->update(registry, deltaTime);
            }

            editorUI->beginFrame();
            editorUI->draw(registry);
            editorUI->endFrame();

            window.update();
        }
    }

    void Application::cleanup() {
        if (eventSystem) {
            eventSystem->unsubscribe("collision", collisionSubscription);
        }
    }
}
