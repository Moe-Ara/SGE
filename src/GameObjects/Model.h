#ifndef GLCPP_MODEL_H
#define GLCPP_MODEL_H

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>
#include <string>
#include <gl/glew.h>
//#include "../../external/tiny_object_loader.h"
#include <stdexcept>
#include <memory>
#include <unordered_map>
#include <iostream>
namespace SGE::GAMEOBJECTS {
    class Model {
    public:
        struct Vertex {
            glm::vec3 position{};
            glm::vec3 color{};
            glm::vec3 normal{};
            glm::vec2 uv{};

            bool operator==(const Vertex& other) const {
                return position == other.position && color == other.color &&
                       normal == other.normal && uv == other.uv;
            }
        };

        Model(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
        ~Model();
        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;

        void bind() const;
        void unbind() const;
        void render() const;
        const std::vector<Vertex>& getVertices();


        const std::vector<uint32_t> &getMIndices() const;

        GLuint getMVbo() const;

        GLuint getMIbo() const;

        GLuint getMVao() const;

        GLsizei getMVertexCount() const;

        GLsizei getMIndexCount() const;

        bool isHasIndexBuffer() const;

    private:
        void createVertexBuffer(const std::vector<Vertex>& vertices);
        void createIndexBuffer(const std::vector<uint32_t>& indices);
        std::vector<Vertex> m_vertices;
        std::vector<uint32_t> m_indices ;
        GLuint m_vbo{};
        GLuint m_ibo{};
        GLuint m_vao{};
        GLsizei m_vertexCount{};
        GLsizei m_indexCount{};
        bool hasIndexBuffer{false};
    };
}

// Define the custom hash function for Vertex in the std namespace
namespace std {
    using Vertex = SGE::GAMEOBJECTS::Model::Vertex;
    template <>
    struct hash<Vertex> {
        size_t operator()(const Vertex& v) const {
            size_t hashValue = 0;
            hash_combine(hashValue, v.position.x);
            hash_combine(hashValue, v.position.y);
            hash_combine(hashValue, v.position.z);
            hash_combine(hashValue, v.color.r);
            hash_combine(hashValue, v.color.g);
            hash_combine(hashValue, v.color.b);
            hash_combine(hashValue, v.normal.x);
            hash_combine(hashValue, v.normal.y);
            hash_combine(hashValue, v.normal.z);
            hash_combine(hashValue, v.uv.x);
            hash_combine(hashValue, v.uv.y);
            return hashValue;
        }

    private:
        template <typename T>
        static void hash_combine(size_t& seed, const T& v) {
            std::hash<T> hasher;
            seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
    };
}

#endif //GLCPP_MODEL_H
