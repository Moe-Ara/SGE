//
// Created by Mohamad on 30/06/2024.
//

#include <glm/ext/matrix_transform.hpp>
#include "ThirdPersonCamera.h"

namespace SGE::graphics {

    ThirdPersonCamera::ThirdPersonCamera(float distance, float vertAngle, float horizAngle)
            : distanceFromTarget(distance), verticalAngle(vertAngle), horizontalAngle(horizAngle) {

        // You can initialize the camera with a default position and rotation here
    }

    void ThirdPersonCamera::update(glm::vec3 playerPosition, float deltaTime) {
        // Calculate the offset from the player based on input angles
        glm::vec3 offset;
        offset.x = distanceFromTarget * sin(glm::radians(horizontalAngle)) * cos(glm::radians(verticalAngle));
        offset.y = distanceFromTarget * sin(glm::radians(verticalAngle));
        offset.z = distanceFromTarget * cos(glm::radians(horizontalAngle)) * cos(glm::radians(verticalAngle));
        position = playerPosition + offset;

        // Look at the player
        lookAtTarget = playerPosition;
        viewMatrix = glm::lookAt(position, lookAtTarget, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    void ThirdPersonCamera::setCameraAngles(float newVerticalAngle, float newHorizontalAngle) {
        verticalAngle = newVerticalAngle;
        horizontalAngle = newHorizontalAngle;
    }
} // graphics