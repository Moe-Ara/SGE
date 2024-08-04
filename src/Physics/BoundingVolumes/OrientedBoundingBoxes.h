//
// Created by Mohamad on 15/07/2024.
//

#ifndef GLCPP_ORIENTEDBOUNDINGBOXES_H
#define GLCPP_ORIENTEDBOUNDINGBOXES_H

#include <glm/fwd.hpp>
#include <glm/detail/type_mat3x3.hpp>
#include "../BoundingVolume.h"
#include "../../GameObjects/Transform.h"


namespace SGE::PHYSICS {

        class OrientedBoundingBoxes : public BoundingVolume{
        public:
            OrientedBoundingBoxes(const SGE::GAMEOBJECTS::Transform& transform, const glm::vec3& halfSizes, const glm::mat3& orientation);

            ~OrientedBoundingBoxes() override = default;

            bool intersects(const BoundingVolume &other) const override;

            ColliderType getType() const override;

            glm::vec3 getCenter() const override;
            glm::vec3 getHalfSizes() const { return m_halfSizes; }

            void render( GRAPHICS::Shader &shader)  override;

            glm::mat3 getOrientation() const { return m_orientation; }
        private:
            glm::vec3 m_center;
            glm::vec3 m_halfSizes;
            glm::mat3 m_orientation;
            const SGE::GAMEOBJECTS::Transform m_transform;

            bool intersectsWithOBB(const OrientedBoundingBoxes &other) const;



        };

    } // PHYSICS
// SGE

#endif //GLCPP_ORIENTEDBOUNDINGBOXES_H
