//
// Created by Mohamad on 09/07/2024.
//

#ifndef GLCPP_MATERIAL_H
#define GLCPP_MATERIAL_H

#include <glm/glm.hpp>
namespace SGE::UTILS{
struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;

    // Texture properties
    bool hasDiffuseTexture;
    bool hasSpecularTexture;
    bool hasNormalTexture;

    Material() :
        ambient(1.0f, 1.0f, 1.0f),
        diffuse(1.0f, 1.0f, 1.0f),
        specular(1.0f, 1.0f, 1.0f),
        shininess(32.0f),
        hasDiffuseTexture(false),
        hasSpecularTexture(false),
        hasNormalTexture(false) {}
};}
#endif //GLCPP_MATERIAL_H

