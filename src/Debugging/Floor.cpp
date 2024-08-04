//
// Created by Mohamad on 13/07/2024.
//

#include "Floor.h"



    namespace SGE::DEBUGGING {
        Floor::Floor() : SGE::GAMEOBJECTS::GameObject(SGE::GAMEOBJECTS::Transform{},SGE::UTILS::ModelLoader::loadModelFromFile("Ground.obj")){
            this->transform.translate({0.f,3.f,0.f});
            m_collider= std::make_shared<SGE::PHYSICS::PlaneCollider>();
        }

        Floor::~Floor() = default;

        glm::vec3 Floor::getColor() const {
            return {200.f,200.f,200.f};
        }

        std::shared_ptr<SGE::PHYSICS::Collider> Floor::getCollider() {
            return m_collider;
        }
    } // DEBUGGING
// SGE