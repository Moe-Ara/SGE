//
// Created by Mohamad on 11/07/2024.
//

#include <glm/gtc/quaternion.hpp>
#include "Transform.h"
namespace SGE::GAMEOBJECTS{
    glm::mat4 Transform::mat4() const {
        glm::mat4 transformMatrix = glm::mat4(1.0f);
        transformMatrix = glm::translate(transformMatrix, m_translation);
        transformMatrix *= glm::mat4_cast(m_rotation);
        transformMatrix = glm::scale(transformMatrix, m_scale);
        return transformMatrix;
    }

    glm::mat3 Transform::normalMatrix() const {
        glm::mat4 transformMatrix = mat4();
        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(transformMatrix)));

        // Checking the determinant to avoid returning an invalid matrix
        if (glm::determinant(glm::mat3(transformMatrix)) == 0.0f) {
            return glm::mat3(1.0f);
        }

        return normalMatrix;
    }


    void Transform::translate(const glm::vec3& deltaTranslation) {
        m_translation += deltaTranslation;
    }

    void Transform::scale(const glm::vec3& deltaScale) {
        m_scale = deltaScale;
    }

    void Transform::rotate(const glm::quat& deltaRotation) {
        m_rotation = deltaRotation;
    }

    const glm::vec3 &Transform::getMTranslation() const {
        return m_translation;
    }

    const glm::vec3 &Transform::getMScale() const {
        return m_scale;
    }

    const glm::quat &Transform::getMRotation() const {
        return m_rotation;
    }

}

