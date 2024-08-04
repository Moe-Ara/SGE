//
// Created by Mohamad on 13/07/2024.
//

#ifndef GLCPP_COLLIDER_H
#define GLCPP_COLLIDER_H


#include <glm/vec3.hpp>
#include "ColliderType.h"
#include "../GameObjects/Transform.h"
#include "BoundingVolume.h"
#include <memory>
namespace SGE::PHYSICS {

        class Collider {
        public:
            virtual ~Collider()=default;
            virtual bool checkCollision(Collider& other)=0;
            virtual void update(const SGE::GAMEOBJECTS::Transform &position)=0;
            virtual void render() const = 0;

            [[maybe_unused]] virtual const ColliderType getType()=0;
            [[nodiscard]] virtual SGE::GAMEOBJECTS::Transform getTransform() const = 0;
            [[nodiscard]] virtual std::shared_ptr<BoundingVolume> getOBB() const =0;
        };

    } // PHYSICS
// SGE

#endif //GLCPP_COLLIDER_H
