//
// Created by Mohamad on 29/04/2024.
//

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "sge_camera.h"


void
SGE::graphics::sge_camera::setOrthographicProjection(float left, float right, float top, float bottom, float near,
                                                     float far) {
    projectionMatrix = glm::mat4{1.0f};
    projectionMatrix[0][0] = 2.f / (right - left);
    projectionMatrix[1][1] = 2.f / (bottom - top);
    projectionMatrix[2][2] = 1.f / (far - near);
    projectionMatrix[3][0] = -(right + left) / (right - left);
    projectionMatrix[3][1] = -(bottom + top) / (bottom - top);
    projectionMatrix[3][2] = -near / (far - near);

}

void SGE::graphics::sge_camera::setPrespectiveProjection(float fovy, float aspect, float near, float far) {
    assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
    const float tanHalfFovy = tan(fovy / 2.f);
    projectionMatrix = glm::mat4{0.0f};
    projectionMatrix[0][0] = 1.f / (aspect * tanHalfFovy);
    projectionMatrix[1][1] = 1.f / (tanHalfFovy);
    projectionMatrix[2][2] = far / (far - near);
    projectionMatrix[2][3] = 1.f;
    projectionMatrix[3][2] = -(far * near) / (far - near);
}

void SGE::graphics::sge_camera::setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
    const glm::vec3 w{glm::normalize(direction)};
    const glm::vec3 u{glm::normalize(glm::cross(w, up))};
    const glm::vec3 v{glm::cross(w, u)};

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
}

void SGE::graphics::sge_camera::setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
    setViewDirection(position, target - position, up);
}

void SGE::graphics::sge_camera::setViewYXZ(glm::vec3 position, glm::vec3 rotation) {
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
}

const glm::mat4 &SGE::graphics::sge_camera::getProjection() const {
    return projectionMatrix;

}

const glm::mat4 &SGE::graphics::sge_camera::getView() const {
    return viewMatrix;
}

glm::vec3 SGE::graphics::sge_camera::getRight() const {
    return glm::vec3(this->getView()[0]);
}

glm::vec3 SGE::graphics::sge_camera::getRelativeUp() const {
    return glm::vec3(this->getView()[1]);
}

glm::vec3 SGE::graphics::sge_camera::getForward() const {
    return glm::vec3(this->getView()[2]);
}

glm::vec3 SGE::graphics::sge_camera::getAbsoluteUp() const {
    return glm::vec3(0.f,-1.f,0.f);
}

glm::vec3 SGE::graphics::sge_camera::getPositionFromViewMatrix() {
    // Extract the position from the fourth column of the view matrix
    return glm::vec3(getView()[3]);
}

glm::mat3 SGE::graphics::sge_camera::getRotationFromViewMatrix() {
    // Extract the orientation vectors (forward, up, and right) from the view matrix
    glm::vec3 forward(getView()[0][2], getView()[1][2], getView()[2][2]);
    glm::vec3 up(getView()[0][1], getView()[1][1], getView()[2][1]);
    glm::vec3 right(getView()[0][0], getView()[1][0], getView()[2][0]);

    // Construct a rotation matrix from the orientation vectors
    return glm::mat3(right, up, -forward); // Note: Negate the forward vector to convert from left-handed to right-handed coordinate system
}

