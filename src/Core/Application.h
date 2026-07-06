#ifndef GLCPP_APPLICATION_H
#define GLCPP_APPLICATION_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>
#include "../GameObjects/GameObject.h"
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../GameObjects/Model.h"
#include "../Graphics/Window.h"
#include "../Input/InputHandler.h"
#include "../Graphics/Shader.h"
#include "../Graphics/Camera.h"
#include "../Debugging/Player.h"
#include "../Debugging/npc.h"
#include "../Utils/Material.h"
#include "../Utils/Light.h"
#include "../Debugging/CoordinateSystem.h"
#include <chrono>
#include "../Exceptions/SGE_Exception.h"
#include "../Graphics/Scene.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/ThirdPersonCamera.h"
#include "../Events/Event.h"
#include "../Events/EventSystem.h"
#include "../Physics/PhysicsEngine.h"

namespace SGE::CORE {
    class Application {
    public:
        Application();

        void run();

        virtual ~Application();

    private:
        void setup();
        bool initialize();
        void gameLoop();
        void initWindow();
        void initScene();
        void initInputHandler();
        void setupEventHandlers();
        void handleCollision(const SGE::EVENTS::Event& event);
        void processInput();
        void render();
        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
        void cleanup();

        std::shared_ptr<SGE::INPUT::InputHandler> m_inputHandler;
        std::shared_ptr<SGE::GRAPHICS::Camera> m_mainCamera;
        std::shared_ptr<SGE::GRAPHICS::Shader> m_shaderProgram;
        std::shared_ptr<SGE::GRAPHICS::Scene> m_scene;
        std::shared_ptr<SGE::GRAPHICS::Renderer> m_renderer;

        std::shared_ptr<SGE::DEBUGGING::Player> m_player_ptr;
        std::shared_ptr<SGE::DEBUGGING::npc> m_npc_ptr;
        GLFWwindow* window{nullptr};
        std::shared_ptr<SGE::PHYSICS::PhysicsEngine> physicsEngine;
        std::shared_ptr<SGE::EVENTS::EventSystem> eventSystem;
    };
}

#endif //GLCPP_APPLICATION_H

