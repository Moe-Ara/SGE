#ifndef SGE_UTILS_CAMERAMATH_H
#define SGE_UTILS_CAMERAMATH_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/trigonometric.hpp>
#include <cmath>

namespace SGE::UTILS {

    [[nodiscard]] inline glm::vec3 worldUp() {
        return glm::vec3{0.0f, 1.0f, 0.0f};
    }

    [[nodiscard]] inline glm::vec3 forwardFromYawPitch(float yawDegrees, float pitchDegrees) {
        return glm::normalize(glm::vec3{
            std::cos(glm::radians(yawDegrees)) * std::cos(glm::radians(pitchDegrees)),
            std::sin(glm::radians(pitchDegrees)),
            std::sin(glm::radians(yawDegrees)) * std::cos(glm::radians(pitchDegrees))
        });
    }

    [[nodiscard]] inline glm::vec3 cameraForward(const glm::quat& rotation) {
        return glm::normalize(rotation * glm::vec3{0.0f, 0.0f, -1.0f});
    }

    [[nodiscard]] inline glm::quat lookRotation(const glm::vec3& forward) {
        return glm::quatLookAt(glm::normalize(forward), worldUp());
    }

} // namespace SGE::UTILS

#endif // SGE_UTILS_CAMERAMATH_H
