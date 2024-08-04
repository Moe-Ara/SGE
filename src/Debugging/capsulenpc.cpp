//
// Created by Mohamad on 28/07/2024.
//

#include "capsulenpc.h"


    namespace SGE::DEBUGGING {
        capsulenpc::~capsulenpc() {

        }

        capsulenpc::capsulenpc(long id) : Actor(id, SGE::GAMEOBJECTS::Transform{},
                                          SGE::UTILS::ModelLoader::loadModelFromFile("capsule.obj")) {
            modelPtr = SGE::UTILS::ModelLoader::loadModelFromFile("capsule.obj");
//        this->getTransform().translate(glm::vec3(-10.f, 0.f, 2.f));
//        this->getTransform().scale(glm::vec3(2.f, 2.f, 2.f));
            m_collider= std::make_shared<SGE::PHYSICS::CapsuleCollider>();

        }

        void capsulenpc::move(glm::vec3 movement, float deltaTime) {
        }

        glm::vec3 capsulenpc::getColor() const {
            return {100.0f, 1.0f, 255.0f};
        }

        std::shared_ptr<SGE::PHYSICS::Collider> capsulenpc::getCollider() {
            return this->m_collider;
        }
    } // DEBUGGING
// SGE