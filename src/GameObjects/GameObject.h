//
// Created by Mohamad on 11/07/2024.
//

#ifndef GLCPP_GAMEOBJECT_H
#define GLCPP_GAMEOBJECT_H

#include <memory>
#include "Transform.h"
#include "Model.h"
#include "../Graphics/Shader.h"

namespace SGE::GAMEOBJECTS{
class GameObject {
public:
    GameObject(    Transform transform,
    std::shared_ptr<Model> model);
    virtual ~GameObject();

    Transform& getTransform();
    virtual void update(float deltaTime);
    virtual void render();
    virtual glm::vec3 getColor() const=0;

protected:
    Transform transform;
    std::shared_ptr<Model> model;
};
};



#endif //GLCPP_GAMEOBJECT_H
