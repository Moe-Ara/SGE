//
// Created by Mohamad on 11/07/2024.
//

#include <glm/ext/matrix_transform.hpp>
#include "CoordinateSystem.h"

SGE::DEBUGGING::CoordinateSystem::CoordinateSystem(SGE::GRAPHICS::Shader &shader) : shader(shader) {
    setupAxisBuffers();
}

SGE::DEBUGGING::CoordinateSystem::~CoordinateSystem() {
    glDeleteVertexArrays(1, &axisVAO);
    glDeleteBuffers(1, &axisVBO);
}

void SGE::DEBUGGING::CoordinateSystem::render(const glm::mat4 &view, const glm::mat4 &projection) {
    shader.enable();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-0.9f, -0.9f, 0.0f)); // Translate to bottom-left corner
    model = glm::scale(model, glm::vec3(2.1f, 2.1f, 2.1f)); // Scale down

    shader.setUniformMat4("transform", model);
    shader.setUniformMat4("view", view);
    shader.setUniformMat4("projection", projection);

    glBindVertexArray(axisVAO);
    glDrawArrays(GL_LINES, 0, 6); // Draw the 3 lines (X, Y, Z)
    glBindVertexArray(0);
}

void SGE::DEBUGGING::CoordinateSystem::setupAxisBuffers() {
    GLfloat axisVertices[] = {
            // X axis (red)
            0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            // Y axis (green)
            0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            // Z axis (blue)
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };

    glGenVertexArrays(1, &axisVAO);
    glGenBuffers(1, &axisVBO);

    glBindVertexArray(axisVAO);

    glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *) (3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}
