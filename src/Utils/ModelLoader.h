//
// Created by Mohamad on 11/07/2024.
//

#ifndef GLCPP_MODELLOADER_H
#define GLCPP_MODELLOADER_H
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>
#include <string>
#include <gl/glew.h>
//#include "../../external/tiny_object_loader.h"
#include "../GameObjects/Model.h"
#include <stdexcept>
#include <memory>
#include <unordered_map>
#include <iostream>
namespace SGE::UTILS{
    class ModelLoader {
    public:
        static std::unique_ptr<SGE::GAMEOBJECTS::Model> loadModelFromFile(const std::string& filepath);
    };

}


#endif //GLCPP_MODELLOADER_H
