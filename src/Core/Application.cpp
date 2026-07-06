#include "Application.h"
#include "../Graphics/Scene.h"
#include "../Graphics/Shader.h"
#include "../Physics/PhysicsEngine.h"
#include "../Events/EventSystem.h"
#include "../GameObjects/Actor.h"
#include <iostream>

namespace SGE::CORE {

    void errorCallback(int error, const char *description) {
        std::cerr << "Error: " << description << std::endl;
    }

    Application::Application()
            : m_inputHandler(nullptr), m_mainCamera(nullptr),
              m_shaderProgram(nullptr), m_scene(nullptr), m_renderer(nullptr), window(nullptr), physicsEngine(nullptr), eventSystem(nullptr) {
        setup();
    }

    void Application::setup() {
        initWindow();
        if (!SGE::GRAPHICS::Window::getInstance().isHeadless()) {
            initInputHandler();
            initScene();
        }
    }

    void Application::run() {
        if (!initialize()) {
            return;
        }

        if (SGE::GRAPHICS::Window::getInstance().isHeadless()) {
            std::cout << "Headless mode: skipping game loop." << std::endl;
            return;
        }

        gameLoop();
    }

    void Application::gameLoop() {
        auto lastFrameTime = std::chrono::high_resolution_clock::now();
        auto &window = SGE::GRAPHICS::Window::getInstance();
        while (!window.closed()) {
            if (m_inputHandler->isKeyPressed(GLFW_KEY_ESCAPE)) {
                glfwSetWindowShouldClose(window.getMWindow(), GLFW_TRUE);
                break;
            }

            auto currentFrameTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> deltaTimeDuration = currentFrameTime - lastFrameTime;
            float deltaTime = deltaTimeDuration.count();

            m_scene->update(deltaTime);
            m_mainCamera->update(deltaTime, m_player_ptr);

            window.clear();
            std::vector<std::shared_ptr<SGE::GAMEOBJECTS::Actor>> actors;
            m_renderer->render(m_mainCamera, actors);

            window.update();
            lastFrameTime = currentFrameTime;
        }
    }

    Application::~Application() {
        cleanup();
        glfwTerminate();
    }

    void Application::initWindow() {
        glfwSetErrorCallback(errorCallback);

        auto& windowInstance = SGE::GRAPHICS::Window::getInstance("SGE", 960, 540);
        if (windowInstance.isHeadless()) {
            return;
        }

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDisable(GL_BLEND);
        glClearColor(0.f, 0.2f, 0.3f, 1.0f);
    }

    void Application::initScene() {
        m_mainCamera = std::make_shared<SGE::GRAPHICS::ThirdPersonCamera>(glm::vec3(10.f, 2.f, 0.f));
        m_shaderProgram = std::make_shared<SGE::GRAPHICS::Shader>("vertex.vert", "fragment.frag");
        m_renderer = std::make_shared<SGE::GRAPHICS::Renderer>(m_shaderProgram, m_mainCamera);
        m_scene = std::make_shared<SGE::GRAPHICS::Scene>();

        m_player_ptr = std::make_shared<SGE::DEBUGGING::Player>(123, *m_mainCamera);
        m_npc_ptr = std::make_shared<SGE::DEBUGGING::npc>(0);

        m_scene->addObject(m_player_ptr);
        m_scene->addObject(m_npc_ptr);
    }

    void Application::initInputHandler() {
        const std::vector<int> keysToMonitor = {GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_W, GLFW_KEY_D, GLFW_KEY_LEFT_CONTROL,
                                                GLFW_KEY_LEFT_SHIFT, GLFW_KEY_Q, GLFW_KEY_E, GLFW_KEY_ESCAPE};
        const std::vector<int> buttonsToMonitor = {GLFW_MOUSE_BUTTON_RIGHT, GLFW_MOUSE_BUTTON_LEFT};
        SGE::INPUT::InputHandler::setupKeyHandler(SGE::GRAPHICS::Window::getInstance());
        m_inputHandler = std::make_shared<SGE::INPUT::InputHandler>(keysToMonitor, buttonsToMonitor);
    }

    bool Application::initialize() {
        auto& windowInstance = SGE::GRAPHICS::Window::getInstance();
        if (windowInstance.isHeadless()) {
            std::cout << "Headless mode: skipping GLFW/OpenGL initialization." << std::endl;
            return true;
        }

        window = windowInstance.getMWindow();
        if (!window) {
            std::cerr << "Failed to access GLFW window" << std::endl;
            return false;
        }

        // Create physics engine
        physicsEngine = std::make_shared<SGE::PHYSICS::PhysicsEngine>();

        // Create event system
        eventSystem = std::make_shared<SGE::EVENTS::EventSystem>();

        // Setup event handlers
        setupEventHandlers();

        return true;
    }

    void Application::setupEventHandlers() {
        // Subscribe to collision events
        eventSystem->subscribe("collision", [this](const SGE::EVENTS::IEvent& event) {
            handleCollision(static_cast<const SGE::EVENTS::Event&>(event));
        });
    }

    void Application::handleCollision(const SGE::EVENTS::Event& event) {
        // Process collision events
        std::cout << "Collision detected!" << std::endl;
    }

    void Application::processInput() {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    void Application::render() {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void Application::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }

    void Application::cleanup() {
        if (window) {
            glfwTerminate();
        }
    }
}

