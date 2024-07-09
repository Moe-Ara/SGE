//
// Created by Mohamad on 09/07/2024.
//

#ifndef GLCPP_MATERIAL_H
#define GLCPP_MATERIAL_H

#include <glm/vec3.hpp>
namespace SGE::utils{
struct Material{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};}
#endif //GLCPP_MATERIAL_H
