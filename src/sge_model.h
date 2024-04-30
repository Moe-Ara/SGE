//
// Created by Mohamad on 17/04/2024.
//

#ifndef GLCPP_SGE_MODEL_H
#define GLCPP_SGE_MODEL_H

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>
#include <string>
#include <gl/glew.h>
#include "../external/tiny_object_loader.h"
#include <stdexcept>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <iostream>

namespace SGE::actors{
    class sge_model {
    public:
        struct Vertex{
            glm::vec3 position{};
            glm::vec3 color{};
            glm::vec3 normal{};
            glm::vec2 uv{};

            bool operator==(const Vertex& other) const{
                return position==other.position && color==other.color&& normal==other.normal&& uv==other.uv;
            }

        };
        struct Builder{
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};
            void loadModel(const std::string &filepath);
        };
        sge_model(const Builder &builder);
        ~sge_model();
        sge_model(const sge_model&)=delete;
        sge_model &operator=(const sge_model&)=delete;

        static std::unique_ptr<sge_model> createModelFromFile(const std::string& filepath);
        void bind();
        void unbind();
        void render();

    private:
        GLuint vbo{};
    public:
        GLuint getVbo() const;

        GLuint getIbo() const;

        GLuint getVao() const;

        GLsizei getVertexCount() const;

        GLsizei getIndexCount() const;

    private:
        // Vertex Buffer Object ID
        GLuint ibo{}; // Index Buffer Object ID
        GLuint vao{}; // Vertex Array Object ID
        GLsizei vertexCount; // Number of indices in the index buffer
        GLsizei indexCount; // Number of indices in the index buffer
        bool hasIndexBuffer= false;
        void createVertexBuffer(const std::vector<Vertex> &vertices);
        void createIndexBuffer(const std::vector<uint32_t> &indices);
    };
}
// Define the custom hash function for Vertex in the std namespace
namespace std {
    using Vertex=SGE::actors::sge_model::Vertex;
    template <>
    struct hash<Vertex> {
        size_t operator()(const Vertex& v) const {
            size_t hashValue = 0;
            // Combine hash values of individual members
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
        // Combine hash values using a common hash_combine function
        template <typename T>
        static void hash_combine(size_t& seed, const T& v) {
            std::hash<T> hasher;
            seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
    };
}


#endif //GLCPP_SGE_MODEL_H
