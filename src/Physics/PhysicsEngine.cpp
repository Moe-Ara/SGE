//
// Created by Mohamad on 12/07/2024.
//

#include <glm/gtx/string_cast.hpp>
#include "PhysicsEngine.h"

namespace SGE {
    namespace PHYSICS {
        PhysicsEngine::PhysicsEngine() = default;

        PhysicsEngine::~PhysicsEngine() = default;

        void PhysicsEngine::update(const GRAPHICS::Scene &scene, float deltaTime) {
            for(const auto& object: scene.getGameObjects()){
//                object->applyForce(GRAVITY_FORCE);
                object->updatePhysics(deltaTime);
                if(scene.getGameObjects()[0]->getCollider()->checkCollision(*scene.getGameObjects()[1]->getCollider())){
                    std::cout<< "collided"<<std::endl;
                }
                if(scene.getGameObjects()[1]->getCollider()->checkCollision(*scene.getGameObjects()[0]->getCollider())){
                    std::cout<< "collided"<<std::endl;
                }
                if(scene.getGameObjects()[1]->getCollider()->checkCollision(*scene.getGameObjects()[2]->getCollider())){
                    std::cout<< "collided"<<std::endl;
                }
                if(scene.getGameObjects()[0]->getCollider()->checkCollision(*scene.getGameObjects()[2]->getCollider())){
                    std::cout<< "collided"<<std::endl;
                }

//                if(scene.getGameObjects()[0]->getCollider()->getOBB()->intersects(*scene.getGameObjects()[1]->getCollider()->getOBB())){
//                    std::cout<< "collided With shit"<<std::endl;
//                }

            }



        }
    } // PHYSICS
} // SGE