#ifndef SGE_UTILS_CAMERAMATH_H
#define SGE_UTILS_CAMERAMATH_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/common.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/norm.hpp>
#include <cmath>

namespace SGE::UTILS {

    inline constexpr float kDirectionEpsilon = 0.000001f;

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

    [[nodiscard]] inline float clampPitch(float pitchDegrees) {
        return glm::clamp(pitchDegrees, -89.0f, 89.0f);
    }

    [[nodiscard]] inline glm::vec3 rightFromForward(const glm::vec3& forward) {
        const glm::vec3 normalizedForward = glm::length2(forward) > kDirectionEpsilon
            ? glm::normalize(forward)
            : glm::vec3{0.0f, 0.0f, -1.0f};
        glm::vec3 right = glm::cross(normalizedForward, worldUp());
        if (glm::length2(right) <= kDirectionEpsilon) {
            right = glm::cross(normalizedForward, glm::vec3{0.0f, 0.0f, 1.0f});
        }
        return glm::normalize(right);
    }

    [[nodiscard]] inline glm::vec3 cameraForward(const glm::quat& rotation) {
        return glm::normalize(rotation * glm::vec3{0.0f, 0.0f, -1.0f});
    }

    [[nodiscard]] inline glm::vec3 cameraUp(const glm::quat& rotation) {
        return glm::normalize(rotation * worldUp());
    }

    [[nodiscard]] inline glm::quat lookRotation(const glm::vec3& forward) {
        if (glm::length2(forward) <= kDirectionEpsilon) {
            return glm::quat{1.0f, 0.0f, 0.0f, 0.0f};
        }
        const glm::vec3 normalizedForward = glm::normalize(forward);
        const glm::vec3 right = rightFromForward(normalizedForward);
        const glm::vec3 up = glm::normalize(glm::cross(right, normalizedForward));
        return glm::normalize(glm::quatLookAt(normalizedForward, up));
    }

    [[nodiscard]] inline glm::quat rotationFromYawPitch(float yawDegrees, float pitchDegrees) {
        return lookRotation(forwardFromYawPitch(yawDegrees, pitchDegrees));
    }

    [[nodiscard]] inline glm::vec2 yawPitchFromForward(const glm::vec3& forward) {
        const glm::vec3 normalizedForward = glm::length2(forward) > kDirectionEpsilon
            ? glm::normalize(forward)
            : glm::vec3{0.0f, 0.0f, -1.0f};
        return {
            glm::degrees(glm::atan(normalizedForward.z, normalizedForward.x)),
            glm::degrees(glm::asin(glm::clamp(normalizedForward.y, -1.0f, 1.0f)))
        };
    }

} // namespace SGE::UTILS

#endif // SGE_UTILS_CAMERAMATH_H
