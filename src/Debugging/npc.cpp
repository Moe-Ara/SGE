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
                              SGE::UTILS::ModelLoader::loadModelFromFile("cube.obj")) {
        modelPtr = SGE::UTILS::ModelLoader::loadModelFromFile("cube.obj");
        this->getTransform().translate(glm::vec3(10.f, 0.f, 2.f));
    }

    void npc::move(glm::vec3 movement, float deltaTime) {
    }

    glm::vec3 npc::getColor() const {
        return {1.0f, 1.0f, 1.0f};
    }

}
