//
// Created by Mohamad on 30/06/2024.
//

#include <glm/ext/matrix_transform.hpp>
#include "ThirdPersonCamera.h"

namespace SGE::graphics {

    ThirdPersonCamera::ThirdPersonCamera(float distance, float vertAngle, float horizAngle)
            : Camera(), distanceFromTarget(distance), verticalAngle(vertAngle), horizontalAngle(horizAngle) {

        // You can initialize the camera with a default position and rotation here
    }

    void ThirdPersonCamera::update(glm::vec3 playerPosition, float deltaTime) {
        // Calculate the offset from the player based on input angles
//        glm::vec3 offset;
//        offset.x = distanceFromTarget * sin(glm::radians(horizontalAngle)) * cos(glm::radians(verticalAngle));
//        offset.y = distanceFromTarget * sin(glm::radians(verticalAngle));
//        offset.z = distanceFromTarget * cos(glm::radians(horizontalAngle)) * cos(glm::radians(verticalAngle));
//        position = playerPosition + offset;
//
//        // Look at the player
//        lookAtTarget = playerPosition;
//        viewMatrix = glm::lookAt(position, lookAtTarget, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec3 offset = glm::vec3(0.0f, 2.0f, 10.0f);  // Position camera behind and above the player
        glm::vec3 cameraPosition = playerPosition + offset;
        glm::vec3 lookAtPosition = playerPosition;  // Ensure the camera is looking at the player

        setViewDirection(cameraPosition, lookAtPosition, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    void ThirdPersonCamera::setCameraAngles(float newVerticalAngle, float newHorizontalAngle) {
        verticalAngle = newVerticalAngle;
        horizontalAngle = newHorizontalAngle;
    }

    float ThirdPersonCamera::getDistanceFromTarget() const {
        return distanceFromTarget;
    }

    float ThirdPersonCamera::getVerticalAngle() const {
        return verticalAngle;
    }

    float ThirdPersonCamera::getHorizontalAngle() const {
        return horizontalAngle;
    }

    const glm::vec3 &ThirdPersonCamera::getPosition() const {
        return position;
    }

    const glm::vec3 &ThirdPersonCamera::getLookAtTarget() const {
        return lookAtTarget;
    }

    const glm::vec3 &ThirdPersonCamera::getUpDirection() const {
        return upDirection;
    }

    const glm::mat4 &ThirdPersonCamera::getViewMatrix() const {
        return viewMatrix;
    }
} // graphics