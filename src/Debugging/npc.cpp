//
// Created by Mohamad on 07/07/2024.
//

#include "npc.h"
#include "../GameObjects/Transform.h"
#include "../GameObjects/Model.h"

namespace SGE::DEBUGGING {


    npc::~npc() {

    }

    npc::npc(long id) : Actor(id, SGE::GAMEOBJECTS::Transform{},
                              SGE::UTILS::ModelLoader::loadModelFromFile("sphere.obj")) {
        modelPtr = SGE::UTILS::ModelLoader::loadModelFromFile("sphere.obj");
        this->getTransform().translate(glm::vec3(10.f, 0.f, 2.f));
//        this->getTransform().scale(glm::vec3(2.f, 2.f, 2.f));
        m_collider= std::make_shared<SGE::PHYSICS::SphereCollider>();

    }

    void npc::move(glm::vec3 movement, float deltaTime) {
    }

    glm::vec3 npc::getColor() const {
        return {100.0f, 1.0f, 255.0f};
    }

    std::shared_ptr<SGE::PHYSICS::Collider> npc::getCollider() {
        return this->m_collider;
    }

}
