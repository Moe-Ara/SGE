//
// Created by Mohamad on 11/07/2024.
//
#define TINYOBJLOADER_IMPLEMENTATION
#include "ModelLoader.h"


#include "../../external/tiny_object_loader.h"
#include "cassert"
#include "../GameObjects/Model.h"

std::unique_ptr <SGE::GAMEOBJECTS::Model> SGE::UTILS::ModelLoader::loadModelFromFile(const std::string &filepath) {


    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
        throw std::runtime_error(warn + err);
    }
    std::vector<SGE::GAMEOBJECTS::Model::Vertex> vertices;
    std::vector<uint32_t> indices;
    std::unordered_map<SGE::GAMEOBJECTS::Model::Vertex, uint32_t> uniqueVertices;
    vertices.clear();
    indices.clear();


    for (const auto &shape : shapes) {
        for (const auto &index : shape.mesh.indices) {

            SGE::GAMEOBJECTS::Model::Vertex vertex{};

            if (index.vertex_index >= 0) {
                vertex.position = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                };
                if (!attrib.colors.empty()) {
                    vertex.color = {
                            attrib.colors[3 * index.vertex_index + 0],
                            attrib.colors[3 * index.vertex_index + 1],
                            attrib.colors[3 * index.vertex_index + 2]
                    };
                }
            }
            if (index.normal_index >= 0) {
                vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                };
            }
            if (index.texcoord_index >= 0) {
                vertex.uv = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        attrib.texcoords[2 * index.texcoord_index + 1]
                };
            }

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }
            indices.push_back(uniqueVertices[vertex]);
        }}

        return std::make_unique<SGE::GAMEOBJECTS::Model>(vertices, indices);

}
