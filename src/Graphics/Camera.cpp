//
// Created by Mohamad on 29/04/2024.
//

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/norm.hpp>
#include <cmath>
#include <limits>
#include <stdexcept>
#include "Camera.h"

namespace SGE::GRAPHICS {

Camera::Camera(glm::vec3 position) : position(position) {
    setViewDirection(position, direction, upVec);
}

void
SGE::GRAPHICS::Camera::setOrthographicProjection(float left, float right, float top, float bottom, float near,
                                                 float far) {
    projectionMatrix = glm::mat4{1.0f};
    projectionMatrix[0][0] = 2.f / (right - left);
    projectionMatrix[1][1] = 2.f / (top - bottom);
    projectionMatrix[2][2] = -2.f / (far - near);
    projectionMatrix[3][0] = -(right + left) / (right - left);
    projectionMatrix[3][1] = -(top + bottom) / (top - bottom);
    projectionMatrix[3][2] = -(far + near) / (far - near);

}

void SGE::GRAPHICS::Camera::setPrespectiveProjection(float fovy, float aspect, float near, float far) {
    setPerspectiveProjection(fovy, aspect, near, far);
}

void SGE::GRAPHICS::Camera::setPerspectiveProjection(float fovy, float aspect, float near, float far) {
    if (!std::isfinite(fovy) || !std::isfinite(aspect) || !std::isfinite(near) ||
        !std::isfinite(far) || fovy <= 0.0f || fovy >= glm::pi<float>() ||
        aspect <= std::numeric_limits<float>::epsilon() || near <= 0.0f || far <= near) {
        throw std::invalid_argument("Invalid perspective projection parameters");
    }
    const float tanHalfFovy = tan(fovy / 2.f);
    projectionMatrix = glm::mat4{0.0f};
    projectionMatrix[0][0] = 1.f / (aspect * tanHalfFovy);
    projectionMatrix[1][1] = 1.f / (tanHalfFovy);
    projectionMatrix[2][2] = -(far + near) / (far - near);
    projectionMatrix[2][3] = -1.f;
    projectionMatrix[3][2] = -(2.f * far * near) / (far - near);
}

void SGE::GRAPHICS::Camera::setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
    this->position = position;
    if (glm::length2(direction) <= std::numeric_limits<float>::epsilon()) {
        direction = glm::vec3{0.0f, 0.0f, -1.0f};
    }
    this->direction = glm::normalize(direction);
    if (glm::length2(up) <= std::numeric_limits<float>::epsilon()) {
        up = glm::vec3{0.0f, 1.0f, 0.0f};
    }
    up = glm::normalize(up);
    if (glm::length2(glm::cross(this->direction, up)) <= std::numeric_limits<float>::epsilon()) {
        up = glm::abs(this->direction.y) < 0.999f
            ? glm::vec3{0.0f, 1.0f, 0.0f}
            : glm::vec3{0.0f, 0.0f, 1.0f};
    }
    this->right = glm::normalize(glm::cross(this->direction, up));
    this->up = glm::normalize(glm::cross(this->right, this->direction));
    this->upVec = this->up;

    viewMatrix = glm::lookAt(this->position, this->position + this->direction, this->up);
}

void SGE::GRAPHICS::Camera::setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
    setViewDirection(position, target - position, up);
}

void SGE::GRAPHICS::Camera::setViewYXZ(glm::vec3 position, glm::vec3 rotation) {
    const float c3 = glm::cos(rotation.z);
    const float s3 = glm::sin(rotation.z);
    const float c2 = glm::cos(rotation.x);
    const float s2 = glm::sin(rotation.x);
    const float c1 = glm::cos(rotation.y);
    const float s1 = glm::sin(rotation.y);
    const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
    const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
    const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
    viewMatrix = glm::mat4{1.f};
    viewMatrix[0][0] = u.x;
    viewMatrix[1][0] = u.y;
    viewMatrix[2][0] = u.z;
    viewMatrix[0][1] = v.x;
    viewMatrix[1][1] = v.y;
    viewMatrix[2][1] = v.z;
    viewMatrix[0][2] = w.x;
    viewMatrix[1][2] = w.y;
    viewMatrix[2][2] = w.z;
    viewMatrix[3][0] = -glm::dot(u, position);
    viewMatrix[3][1] = -glm::dot(v, position);
    viewMatrix[3][2] = -glm::dot(w, position);
    this->position = position;
    right = glm::normalize(glm::vec3{viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]});
    up = glm::normalize(glm::vec3{viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]});
    upVec = up;
    direction = -glm::normalize(glm::vec3{viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]});
}

const glm::mat4 &SGE::GRAPHICS::Camera::getProjection() const {
    return projectionMatrix;

}

const glm::mat4 &SGE::GRAPHICS::Camera::getView() const {
    return viewMatrix;
}

glm::vec3 SGE::GRAPHICS::Camera::getRight() const {
    return right;
}

glm::vec3 SGE::GRAPHICS::Camera::getRelativeUp() const {
    return up;
}

glm::vec3 SGE::GRAPHICS::Camera::getForward() const {
    return direction;
}

glm::vec3 SGE::GRAPHICS::Camera::getAbsoluteUp() const {
    return glm::vec3(0.f, 1.f, 0.f);
}

glm::vec3 SGE::GRAPHICS::Camera::getPositionFromViewMatrix() {
    return glm::vec3(glm::inverse(getView())[3]);
}

glm::mat3 SGE::GRAPHICS::Camera::getRotationFromViewMatrix() {
    return glm::mat3(glm::inverse(getView()));
}

void SGE::GRAPHICS::Camera::moveCamera(glm::vec3 position) {
    setViewDirection(position, direction,up);

}

glm::mat4 SGE::GRAPHICS::Camera::getViewMatrix() const {
    return viewMatrix;
}

glm::mat4 SGE::GRAPHICS::Camera::getProjectionMatrix() const {
    return projectionMatrix;
}

} // namespace SGE::GRAPHICS

