#include "Model.h"

namespace SGE::GAMEOBJECTS {

    Model::Model(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices): m_vertices{vertices},m_indices(indices) {
        createVertexBuffer(vertices);
        if (!indices.empty()) {
            createIndexBuffer(indices);
        }
    }

    Model::~Model() {
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ibo);
        glDeleteVertexArrays(1, &m_vao);
    }
    const std::vector<SGE::GAMEOBJECTS::Model::Vertex> &Model::getVertices() {
        return m_vertices;
    }
    void Model::createVertexBuffer(const std::vector<Vertex>& vertices) {
        m_vertexCount = static_cast<GLsizei>(vertices.size());

        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

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
        m_indexCount = static_cast<GLsizei>(indices.size());
        hasIndexBuffer = true;

        glGenBuffers(1, &m_ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
    }

    void Model::bind() const {
        glBindVertexArray(m_vao);
        if (hasIndexBuffer) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        }
    }

    void Model::unbind() const {
        glBindVertexArray(0);
    }

    void Model::render() const {
        bind();
        if (hasIndexBuffer) {
            glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
        } else {
            glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
        }
        unbind();
    }

    const std::vector<uint32_t> &Model::getMIndices() const {
        return m_indices;
    }

    GLuint Model::getMVbo() const {
        return m_vbo;
    }

    GLuint Model::getMIbo() const {
        return m_ibo;
    }

    GLuint Model::getMVao() const {
        return m_vao;
    }

    GLsizei Model::getMVertexCount() const {
        return m_vertexCount;
    }

    GLsizei Model::getMIndexCount() const {
        return m_indexCount;
    }

    bool Model::isHasIndexBuffer() const {
        return hasIndexBuffer;
    }




}

