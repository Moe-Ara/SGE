//
// Created by Mohamad on 11/07/2024.
//

#ifndef GLCPP_GAMEOBJECT_H
#define GLCPP_GAMEOBJECT_H

#include <memory>
#include "Transform.h"
#include "Model.h"
#include "../Graphics/Shader.h"
#include "../Physics/Collider.h"
#include "../Physics/Colliders/SphereCollider.h"

namespace SGE::GAMEOBJECTS{
class GameObject {
public:
    GameObject(    Transform transform,
    std::shared_ptr<Model> model);
    GameObject(const GameObject &) = delete;
    GameObject &operator=(const GameObject &) = delete;
    GameObject(GameObject &&) = default;
    GameObject &operator=(GameObject &&) = default;
    virtual ~GameObject();

    Transform& getTransform();
    virtual void update(float deltaTime);
    virtual void updatePhysics(float deltaTime);
    virtual void applyForce(const glm::vec3& force);
    virtual void render();
    virtual glm::vec3 getColor() const=0;

    virtual std::shared_ptr<SGE::PHYSICS::Collider> getCollider();

protected:
    Transform transform;
    std::shared_ptr<Model> model;
};
};



#endif //GLCPP_GAMEOBJECT_H
