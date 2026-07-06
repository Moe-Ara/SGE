//
// Created by Developer on [date].
//

#ifndef GLCPP_IPHYSICSENGINE_H
#define GLCPP_IPHYSICSENGINE_H

#include <vector>
#include <memory>
#include <glm/glm.hpp>

namespace SGE::PHYSICS {
    class IPhysicsEngine {
    public:
        virtual ~IPhysicsEngine() = default;
        
        // Main update loop
        virtual void update(float deltaTime) = 0;
        
        // Actor management
        virtual void addActor(std::shared_ptr<class Actor> actor) = 0;
        virtual void removeActor(std::shared_ptr<class Actor> actor) = 0;
        
        // Physics properties
        virtual void setGravity(const glm::vec3& gravity) = 0;
        virtual glm::vec3 getGravity() const = 0;
        
        // Collision detection settings
        virtual void setCollisionDetectionMethod(const std::string& method) = 0;
        virtual std::string getCollisionDetectionMethod() const = 0;
    };
}

#endif //GLCPP_IPHYSICSENGINE_H