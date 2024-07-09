//
// Created by Mohamad on 30/06/2024.
//

#ifndef GLCPP_THIRDPERSONCAMERA_H
#define GLCPP_THIRDPERSONCAMERA_H

#include "Camera.h"

namespace SGE::graphics {

    class ThirdPersonCamera:public Camera {
    public:
        ThirdPersonCamera(float distanceFromTarget, float verticalAngle, float horizontalAngle);

        void update(glm::vec3 playerPosition, float deltaTime);
        void setCameraAngles(float newVerticalAngle, float newHorizontalAngle);

    private:
        float distanceFromTarget{5.0f};
    public:
        float getDistanceFromTarget() const;

        float getVerticalAngle() const;

        float getHorizontalAngle() const;

        const glm::vec3 &getPosition() const;

        const glm::vec3 &getLookAtTarget() const;

        const glm::vec3 &getUpDirection() const;

        const glm::mat4 &getViewMatrix() const;

    private:
        float verticalAngle{0.0};
        float horizontalAngle{180.0f};
        glm::vec3 position{0.0f, 2.0f, 5.0f};
        glm::vec3 lookAtTarget{0.0f,0.0f,0.0f};
        glm::vec3 upDirection{0.0f, 1.0f, 0.0f};
        glm::mat4 viewMatrix{1.f};

    };

} // graphics

#endif //GLCPP_THIRDPERSONCAMERA_H
