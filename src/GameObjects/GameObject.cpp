#include "GameObject.h"

namespace SGE::actors {

    glm::mat4 Transform::mat4() const {
        glm::mat4 transformMatrix = glm::mat4(1.0f);
        transformMatrix = glm::translate(transformMatrix, translation);
        transformMatrix *= glm::mat4_cast(rotation);
        transformMatrix = glm::scale(transformMatrix, scale);
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

}
