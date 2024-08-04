//
// Created by Mohamad on 15/07/2024.
//

#include <glm/vec3.hpp>
#include <glm/gtx/string_cast.hpp>
#include "OrientedBoundingBoxes.h"


    namespace SGE::PHYSICS {
        bool OrientedBoundingBoxes::intersects(const BoundingVolume &other) const {

                    return intersectsWithOBB(dynamic_cast<const OrientedBoundingBoxes&>(other));



        }

        ColliderType OrientedBoundingBoxes::getType() const {
            return ColliderType::OBB;
        }

        glm::vec3 OrientedBoundingBoxes::getCenter() const {
            return m_center;
        }

        bool OrientedBoundingBoxes::intersectsWithOBB(const OrientedBoundingBoxes &other) const {
            const glm::vec3& a = m_halfSizes;
            const glm::vec3& b = other.getHalfSizes();

            const glm::mat3& R = m_orientation;
            const glm::mat3& S = other.getOrientation();

            glm::mat3 C = glm::transpose(R) * S;
            glm::vec3 t = glm::transpose(R) * (other.getCenter() - m_center);

            glm::mat3 absC;
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    absC[i][j] = std::abs(C[i][j]) + 1e-6;
                }
            }

            for (int i = 0; i < 3; ++i) {
                if (std::abs(t[i]) > a[i] + glm::dot(b, glm::vec3(absC[i][0], absC[i][1], absC[i][2]))) return false;
                if (std::abs(glm::dot(t, glm::vec3(C[0][i], C[1][i], C[2][i]))) > glm::dot(a, glm::vec3(absC[0][i], absC[1][i], absC[2][i])) + b[i]) return false;
            }

            return true;
        }



        OrientedBoundingBoxes::OrientedBoundingBoxes(const SGE::GAMEOBJECTS::Transform& transform, const glm::vec3 &halfSizes, const glm::mat3 &orientation): m_transform(transform), m_halfSizes(halfSizes), m_orientation(orientation) {
            m_center=m_transform.getMTranslation();
        }

        void OrientedBoundingBoxes::render( GRAPHICS::Shader &shader)  {}


    } // PHYSICS
// SGE