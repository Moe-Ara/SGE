//
// Created by Mohamad on 09/07/2024.
//

#ifndef GLCPP_LIGHT_H
#define GLCPP_LIGHT_H

#include <glm/vec3.hpp>
namespace SGE::utils{
    struct Light {
        glm::vec3 position;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
    };
}

#endif //GLCPP_LIGHT_H
