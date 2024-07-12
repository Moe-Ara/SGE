//
// Created by Mohamad on 12/06/2024.
//

#ifndef GLCPP_ACTOR_H
#define GLCPP_ACTOR_H

#include "GameObject.h"
#include "Transform.h"

namespace SGE::GAMEOBJECTS {
    class Actor : public GameObject {

    public:
        Actor(long ID, Transform transform,
              std::shared_ptr<Model> model);

        virtual ~Actor() = default;

        virtual void move(glm::vec3 movement, float deltaTime);

        virtual void applyForce(const glm::vec3 &force);

        virtual void update(float deltaTime);

        //DEBUGGING PURPOSES REMOVE LATER
        [[nodiscard]] virtual glm::vec3 getColor() const = 0;

    protected:
        glm::vec3 velocity;
        glm::vec3 accumulatedForces;
        float mass;
        long ID;
    };
}


#endif //GLCPP_ACTOR_H
