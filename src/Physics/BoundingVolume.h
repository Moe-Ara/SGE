//
// Created by Mohamad on 13/07/2024.
//

#ifndef GLCPP_BOUNDINGVOLUME_H
#define GLCPP_BOUNDINGVOLUME_H

#include "ColliderType.h"
#include "../Graphics/Shader.h"
namespace SGE {
    namespace PHYSICS {

        class BoundingVolume {
        public:
            virtual ~BoundingVolume() = default;

            // Method to check if this bounding volume intersects with another
            virtual bool intersects(const BoundingVolume& other) const = 0;
            virtual SGE::PHYSICS::ColliderType getType() const =0;
            virtual glm::vec3 getCenter() const=0;
            virtual void render(SGE::GRAPHICS::Shader& shader) =0;
        };


    } // PHYSICS
} // SGE

#endif //GLCPP_BOUNDINGVOLUME_H
