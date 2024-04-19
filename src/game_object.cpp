//
// Created by Mohamad on 17/04/2024.
//

#include "game_object.h"

glm::mat4 glcpp::Transform::mat4() {
    glm::mat4 transformMatrix = glm::mat4(1.0f);
    transformMatrix = glm::translate(transformMatrix, translation);
    transformMatrix *= glm::mat4_cast(rotation);
    transformMatrix = glm::scale(transformMatrix, scale);
    return transformMatrix;

}

glm::mat3 glcpp::Transform::normalMatrix() {
    glm::mat4 transformMatrix = mat4();
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(transformMatrix)));

    if (glm::determinant(glm::mat3(transformMatrix)) == 0.0f) {
        return glm::mat3(1.0f);
    }

    return normalMatrix;
}
