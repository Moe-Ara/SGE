//
// Created by Mohamad on 12/06/2024.
//

#ifndef GLCPP_PLAYER_H
#define GLCPP_PLAYER_H

#include "../GameObjects/Actor.h"
#include "../Graphics/Camera.h"
#include "../Input/command.h"
#include "../Input/InputHandler.h"
#include "../Graphics/Shader.h"
#include "../Graphics/ThirdPersonCamera.h"
#include "../Utils/ModelLoader.h"
#include "../Physics/Colliders/SphereCollider.h"
#include "../Physics/Colliders/CubeCollider.h"
#include "../Physics/Colliders/CapsuleCollider.h"

namespace SGE::DEBUGGING {
    class Player : public SGE::GAMEOBJECTS::Actor {
    public:
        void jump(float deltaTime);

        void move(glm::vec3 movement, float deltaTime) override;

        void fire(float deltaTime);

        void dash(float deltaTime);

        ~Player() override;

        void update(float deltaTime) override;

        float getMovementSpeed();

        void setMovementSpeed(float speed);

        float getJumpSpeed();

        void setJumpSpeed(float speed);

        Player(long id, SGE::GRAPHICS::Camera &thirdPersonCamera);

        [[nodiscard]] glm::vec3 getColor() const override;

        std::shared_ptr<SGE::PHYSICS::Collider> getCollider() override;

    private:

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
//        Model m_model{};
        double lastMouseX{}, lastMouseY{};
        float yaw{0}, pitch{0};
        glm::vec3 cameraPosition{0.0f, 0.0f, -10.0f};
        float movementSpeed = 500.f;
        float jumpSpeed = 10.f;
        std::shared_ptr<SGE::GAMEOBJECTS::Model> modelPtr;
        glm::vec3 cameraOffset{0, 0, 0};
        glm::vec3 m_movement{};
        SGE::GRAPHICS::Camera &m_thirdPersonCamera;
        std::shared_ptr<SGE::PHYSICS::CapsuleCollider> m_collider;
        void handleMovement(float deltaTime);
    };
}


#endif //GLCPP_PLAYER_H
