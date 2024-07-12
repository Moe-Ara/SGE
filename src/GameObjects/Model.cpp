#include "Model.h"

namespace SGE::GAMEOBJECTS {

    Model::Model(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
        createVertexBuffer(vertices);
        if (!indices.empty()) {
            createIndexBuffer(indices);
        }
    }

    Model::~Model() {
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ibo);
        glDeleteVertexArrays(1, &vao);
    }

    void Model::createVertexBuffer(const std::vector<Vertex>& vertices) {
        vertexCount = static_cast<GLsizei>(vertices.size());

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
        glEnableVertexAttribArray(3);

        glBindVertexArray(0);
    }

    void Model::createIndexBuffer(const std::vector<uint32_t>& indices) {
        indexCount = static_cast<GLsizei>(indices.size());
        hasIndexBuffer = true;

        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
    }

    void Model::bind() const {
        glBindVertexArray(vao);
        if (hasIndexBuffer) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        }
    }

    void Model::unbind() const {
        glBindVertexArray(0);
    }

    void Model::render() const {
        bind();
        if (hasIndexBuffer) {
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        } else {
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        }
        unbind();
    }

}

