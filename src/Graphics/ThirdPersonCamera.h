#ifndef GLCPP_THIRDPERSONCAMERA_H
#define GLCPP_THIRDPERSONCAMERA_H

#include "Camera.h"
#include "../GameObjects/Actor.h"

namespace SGE::GRAPHICS {
    class ThirdPersonCamera : public Camera {
    public:
        explicit ThirdPersonCamera(glm::vec3 offset);

        void update(float deltaTime,std::shared_ptr<SGE::GAMEOBJECTS::Actor> target) override;
        void handleMouseMovement(std::shared_ptr<SGE::GAMEOBJECTS::Actor> target);
        float getYaw() override;

    private:
        float m_yaw{};
        float m_pitch{};
        double m_lastMouseX{};
        double m_lastMouseY{};
        glm::vec3 m_cameraOffset{};
    };
}

#endif //GLCPP_THIRDPERSONCAMERA_H
