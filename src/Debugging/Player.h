//
// Created by Mohamad on 12/06/2024.
//

#ifndef GLCPP_PLAYER_H
#define GLCPP_PLAYER_H

#include "../GameObjects/Actor.h"
#include "../Graphics/Camera.h"
#include "../Input/InputHandler.h"
#include "../Graphics/Shader.h"
#include "../Graphics/ThirdPersonCamera.h"
#include "../Utils/ModelLoader.h"
#include "../Physics/BasePhysicsComponent.h"
namespace SGE::DEBUGGING {
    class Player : public SGE::GAMEOBJECTS::Actor {
    private:
        std::unique_ptr<SGE::PHYSICS::BasePhysicsComponent> physicsComponent;

    public:
        Player(long id, SGE::GRAPHICS::Camera &thirdPersonCamera);

        ~Player() override = default;

        void update(float deltaTime) override;
        void move(glm::vec3 movement, float deltaTime) override;
        void render(const std::shared_ptr<SGE::GRAPHICS::Shader>& shader) override;
        glm::vec3 getColor() const override;
        void jump(float force);
        float getMovementSpeed();
        void setMovementSpeed(float speed);
        float getJumpSpeed();
        void setJumpSpeed(float speed);
        void fire(float deltaTime);
        void dash(float deltaTime);

    private:
        void handleMovement(float deltaTime);
        void handleInput(float deltaTime);

        SGE::INPUT::InputHandler inputHandler{
                {GLFW_KEY_A,
                        GLFW_KEY_S,
                        GLFW_KEY_W,
                        GLFW_KEY_D,
                        GLFW_KEY_LEFT_CONTROL,
                        GLFW_KEY_LEFT_SHIFT,
                        GLFW_KEY_Q,
                        GLFW_KEY_E, GLFW_KEY_SPACE},

                {GLFW_MOUSE_BUTTON_RIGHT,
                        GLFW_MOUSE_BUTTON_LEFT}
        };
//        Model::Builder builder;
//        Model model{};
        double lastMouseX{}, lastMouseY{};
        float yaw{0}, pitch{0};
        glm::vec3 cameraPosition{0.0f, 0.0f, -10.0f};
        float movementSpeed = 500.f;
        float jumpSpeed = 10.f;
        std::shared_ptr<SGE::GAMEOBJECTS::Model> modelPtr;
        glm::vec3 cameraOffset{0, 0, 0};
        glm::vec3 m_movement{};
        SGE::GRAPHICS::Camera &m_thirdPersonCamera;

    };
}


#endif //GLCPP_PLAYER_H

