#include "Application.h"
#include "../Graphics/Scene.h"
#include "../Debugging/Cubenpc.h"
#include "../Debugging/capsulenpc.h"

namespace SGE::CORE {

    void errorCallback(int error, const char *description) {
        std::cerr << "Error: " << description << std::endl;
    }

    Application::Application()
            : m_inputHandler(nullptr), m_mainCamera(nullptr),
              m_shaderProgram(nullptr), m_scene(nullptr), m_renderer(nullptr) {
        setup();
    }

    void Application::setup() {
        initWindow();
        initInputHandler();
        initScene();
    }

    void Application::run() {
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
            SGE::PHYSICS::PhysicsEngine::update(*m_scene, deltaTime);
            window.clear();
            m_renderer->render(*m_scene);

            window.update();
            lastFrameTime = currentFrameTime;
        }
    }

    Application::~Application() {
        glfwTerminate();
    }

    void Application::initWindow() {
        glfwSetErrorCallback(errorCallback);

        auto &window = SGE::GRAPHICS::Window::getInstance("SGE", 960, 540);

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
        std::shared_ptr<SGE::DEBUGGING::Cubenpc> localNpc= std::make_shared<SGE::DEBUGGING::Cubenpc>(0);
        std::shared_ptr<SGE::DEBUGGING::capsulenpc> cnpc= std::make_shared<SGE::DEBUGGING::capsulenpc>(0);
        localNpc->getTransform().translate(glm::vec3 (0.f,0.f,-10.f));
        cnpc->getTransform().translate(glm::vec3 (0.f,0.f,10.f));
        m_player_ptr = std::make_shared<SGE::DEBUGGING::Player>(123, *m_mainCamera);

        m_npc_ptr = std::make_shared<SGE::DEBUGGING::npc>(0);
        auto floor=std::make_shared<SGE::DEBUGGING::Floor>();
        m_scene->addObject(floor);
        m_scene->addObject(m_player_ptr);
        m_scene->addObject(localNpc);

        m_scene->addObject(m_npc_ptr);

        m_scene->addObject(cnpc);

    }

    void Application::initInputHandler() {
        const std::vector<int> keysToMonitor = {GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_W, GLFW_KEY_D, GLFW_KEY_LEFT_CONTROL,
                                                GLFW_KEY_LEFT_SHIFT, GLFW_KEY_Q, GLFW_KEY_E, GLFW_KEY_ESCAPE};
        const std::vector<int> buttonsToMonitor = {GLFW_MOUSE_BUTTON_RIGHT, GLFW_MOUSE_BUTTON_LEFT};
        SGE::INPUT::InputHandler::setupKeyHandler(SGE::GRAPHICS::Window::getInstance());
        m_inputHandler = std::make_shared<SGE::INPUT::InputHandler>(keysToMonitor, buttonsToMonitor);
    }
}
