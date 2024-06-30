//
// Created by Mohamad on 12/06/2024.
//

#ifndef GLCPP_PLAYER_H
#define GLCPP_PLAYER_H

#include "Actor.h"
#include "../Graphics/Camera.h"
#include "../Input/command.h"
#include "../Input/input_handler.h"
#include "../Graphics/Shader.h"
#include "../Graphics/ThirdPersonCamera.h"

namespace SGE::actors{
    class Player : Actor{
    public:
        void jump(float deltaTime) override;
        void move(glm::vec3 movement,float deltaTime) override;
        void fire(float deltaTime) override;
        void dash(float deltaTime) override;
        ~Player() override;
        Transform getTransform() override;
        void update(float deltaTime);
        float getMovementSpeed();
        void setMovementSpeed(float speed);
        float getJumpSpeed();
        void setJumpSpeed(float speed);
        Player();
        void render();

    private:
        Transform transform{};
        SGE::graphics::Camera camera{};
        SGE::Input::input_handler inputHandler{
                {       GLFW_KEY_A,
                        GLFW_KEY_S,
                        GLFW_KEY_W,
                        GLFW_KEY_D,
                        GLFW_KEY_LEFT_CONTROL,
                        GLFW_KEY_LEFT_SHIFT,
                        GLFW_KEY_Q,
                        GLFW_KEY_E,GLFW_KEY_SPACE},

                        {GLFW_MOUSE_BUTTON_RIGHT,
                 GLFW_MOUSE_BUTTON_LEFT}
        };
//        Model::Builder builder;
//        Model model{};
        double lastMouseX{},lastMouseY{};
        float yaw{0},pitch{0};
        glm::vec3 cameraPosition{0.0f, 0.0f, -10.0f};
        float movementSpeed=10.f;
        float jumpSpeed=10.f;
        SGE::graphics::Shader shader{"vertex.vert", "fragment.frag"};
        std::shared_ptr<SGE::actors::Model> modelPtr;
        glm::vec3 cameraOffset{-10,5,-10};


        glm::vec3 calculateDirection(double yaw, double pitch);

        void handleMouseMovement();
    };
}



#endif //GLCPP_PLAYER_H
