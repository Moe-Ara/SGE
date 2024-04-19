//
// Created by Mohamad on 17/04/2024.
//

#define TINYOBJLOADER_IMPLEMENTATION

#include <cassert>
#include "game_model.h"
namespace glcpp{


void game_model::createIndexBuffer(const std::vector<uint32_t> &indices) {
    this->indexCount=static_cast<uint32_t>(indices.size());
    hasIndexBuffer = indexCount > 0;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount*sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void game_model::createVertexBuffer(const std::vector<Vertex> &vertices) {
    vertexCount=static_cast<uint32_t>(vertices.size());
    assert(vertexCount>=3 &&"Vertex Count is less than 3");
    auto bufferSize=sizeof(vertices[0])*vertexCount;
    uint32_t vertexSize=sizeof(vertices[0]);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, bufferSize, vertices.data(),GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(1); // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2); // Normal attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(3); // UV attribute
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}


void game_model::render() {
    glBindVertexArray(vao);

    if (hasIndexBuffer) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }
    glBindVertexArray(0);
    unbind();


}

game_model::game_model(const game_model::Builder &builder) {

    createIndexBuffer(builder.indices);
    createVertexBuffer(builder.vertices);

}

std::unique_ptr<game_model> game_model::createModelFromFile(const std::string &filepath) {
    Builder builder{};
    builder.loadModel(filepath);
    std::cout<<"Vertex Count:"<<builder.vertices.size()<<"\n";
    return std::make_unique<game_model>(builder);
}

    game_model::~game_model() {
        if (vbo != 0) {
            glDeleteBuffers(1, &vbo);
        }
        if (ibo != 0) {
            glDeleteBuffers(1, &ibo);
        }
        if (vao != 0) {
            glDeleteVertexArrays(1, &vao);
        }
    }

    GLuint game_model::getVbo() const {
        return vbo;
    }

    GLuint game_model::getIbo() const {
        return ibo;
    }

    GLuint game_model::getVao() const {
        return vao;
    }

    GLsizei game_model::getVertexCount() const {
        return vertexCount;
    }

    GLsizei game_model::getIndexCount() const {
        return indexCount;
    }

    void game_model::unbind() {

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void game_model::bind() {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);


    }

    void game_model::Builder::loadModel(const std::string &filepath) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
        throw std::runtime_error(warn + err);
    }
    vertices.clear();
    indices.clear();
    std::unordered_map<Vertex, uint32_t> uniqueVertices{};
    for (const auto &shape: shapes) {
        for (const auto &index: shape.mesh.indices) {
            Vertex vertex{};
            if (index.vertex_index >= 0) {
                vertex.position = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2],

                };
                vertex.color = {
                        attrib.colors[3 * index.vertex_index + 0],
                        attrib.colors[3 * index.vertex_index + 1],
                        attrib.colors[3 * index.vertex_index + 2],

                };
            }
            if (index.normal_index >= 0) {
                vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2],

                };

            }
            if (index.texcoord_index >= 0) {
                vertex.uv = {
                        attrib.texcoords[3 * index.texcoord_index + 0],
                        attrib.texcoords[3 * index.texcoord_index + 1],

                };
            }
            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t> (vertices.size());
                vertices.push_back(vertex);

            }
            indices.push_back(uniqueVertices[vertex]);
        }
    }


}}
