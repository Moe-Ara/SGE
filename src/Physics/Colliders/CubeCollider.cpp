//
// Created by Mohamad on 13/07/2024.
//

#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "CubeCollider.h"

#include "../BoundingVolumes/OrientedBoundingBoxes.h"
#include "Collisions.h"
namespace SGE {
    namespace PHYSICS {


        CubeCollider::CubeCollider() : m_cacheDirty(true) {

        }

        CubeCollider::~CubeCollider() {

        }

        bool CubeCollider::checkCollision(Collider &other) {
            switch (other.getType()) {
                case Sphere:
                    return Collisions::intersectsSphereWithCube(dynamic_cast<SphereCollider&>(other),*this);
                case Cube:
                    return Collisions::intersectsCubeWithCube(dynamic_cast<CubeCollider&>(other),*this);
                case Plane:
                    return Collisions::intersectsCubeWithPlane(*this,dynamic_cast<PlaneCollider&>(other));
                case Capsule:
//                    return Collisions::intersectsCubeWithCapsule(*this,dynamic_cast<CapsuleCollider&>(other));
                default:
                    return false;
            }
        }

        void CubeCollider::update(const SGE::GAMEOBJECTS::Transform &transform) {
            if (m_transform!=transform) {
                this->m_transform = transform;
                m_cacheDirty = true;
            }


        }

        void CubeCollider::render() const {
            if (m_model) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                m_model->bind();
                m_model->render();
                m_model->unbind();

                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }

        }

        const ColliderType CubeCollider::getType() {
            return Cube;
        }

        GAMEOBJECTS::Transform CubeCollider::getTransform() const {
            return m_transform;
        }

        std::shared_ptr<BoundingVolume> CubeCollider::getOBB() const {
            auto min = calculateMin();
            auto max = calculateMax();
            glm::vec3 extents = (max - min) / 2.0f;
            glm::vec3 center = (max + min) / 2.0f;
            glm::mat3 orientation = glm::mat3(m_transform.mat4());

            return std::make_shared<OrientedBoundingBoxes>(m_transform, extents, orientation);
        }

        glm::vec3 CubeCollider::calculateMin() const {
            if (m_cacheDirty) {
                updateTransformedVertices();
            }
            glm::vec3 min = glm::vec3(FLT_MAX);

            for (const auto &vertex: m_transformedVertices) {
                min = glm::min(min, vertex);
            }

            return min;
        }

        glm::vec3 CubeCollider::calculateMax() const {
            if (m_cacheDirty) {
                updateTransformedVertices();
            }
            glm::vec3 max = glm::vec3(-FLT_MAX);

            for (const auto &vertex: m_transformedVertices) {
                max = glm::max(max, vertex);
            }

            return max;
        }



        glm::vec3 CubeCollider::calculateLocalMin() const {
            glm::vec3 min = glm::vec3(FLT_MAX);

            for (const auto &vertex: m_model->getVertices()) {
                min = glm::min(min, vertex.position);
            }

            return min;
        }

        glm::vec3 CubeCollider::calculateLocalMax() const {
            glm::vec3 max = glm::vec3(-FLT_MAX);

            for (const auto &vertex: m_model->getVertices()) {
                max = glm::max(max, vertex.position);
            }

            return max;
        }


//        bool CubeCollider::intersectsWithCube(Collider &other) {
//            auto otherCube = dynamic_cast<CubeCollider &>(other);
//            std::vector<glm::vec3> axes;
//            axes.push_back(m_transform.getMRotation() * glm::vec3(1, 0, 0));
//            axes.push_back(m_transform.getMRotation() * glm::vec3(0, 1, 0));
//            axes.push_back(m_transform.getMRotation() * glm::vec3(0, 0, 1));
//            axes.push_back(otherCube.m_transform.getMRotation() * glm::vec3(1, 0, 0));
//            axes.push_back(otherCube.m_transform.getMRotation() * glm::vec3(0, 1, 0));
//            axes.push_back(otherCube.m_transform.getMRotation() * glm::vec3(0, 0, 1));
//
//
//            axes.push_back(glm::cross(m_transform.getMRotation() * glm::vec3(1, 0, 0),
//                                      otherCube.m_transform.getMRotation() * glm::vec3(1, 0, 0)));
//            axes.push_back(glm::cross(m_transform.getMRotation() * glm::vec3(1, 0, 0),
//                                      otherCube.m_transform.getMRotation() * glm::vec3(0, 0, 1)));
//            axes.push_back(glm::cross(m_transform.getMRotation() * glm::vec3(0, 0, 1),
//                                      otherCube.m_transform.getMRotation() * glm::vec3(1, 0, 0)));
//            axes.push_back(glm::cross(m_transform.getMRotation() * glm::vec3(0, 0, 1),
//                                      otherCube.m_transform.getMRotation() * glm::vec3(0, 0, 1)));
//
//            for (const auto &axis: axes) {
//                float minA, maxA;
//                projectCube(axis, minA, maxA);
//
//                float minB, maxB;
//                otherCube.projectCube(axis, minB, maxB);
//
//
//                const float threshold = 0.05f;
//                if (!(maxA + threshold >= minB && maxB + threshold >= minA)) {
//                    return false;
//                }
//            }
//
//            return true;
//        }

        void CubeCollider::projectCube(const glm::vec3 &axis, float &min, float &max) {
            min = std::numeric_limits<float>::max();
            max = -std::numeric_limits<float>::max();

            for (const auto &vertex: getTransformedVertices()) {
                float projection = glm::dot(vertex, axis);
                min = std::min(min, projection);
                max = std::max(max, projection);
            }
        }



        void CubeCollider::updateTransformedVertices() const {
            if (!m_model) {
                return;
            }

            m_transformedVertices.clear();
            const auto &vertices = m_model->getVertices();
            m_transformedVertices.reserve(vertices.size());

            glm::mat4 transformMatrix = m_transform.mat4();

            for (const auto &vertex: vertices) {
                glm::vec3 transformedVertex = glm::vec3(transformMatrix * glm::vec4(vertex.position, 1.0f));
                m_transformedVertices.push_back(transformedVertex);
            }
            m_cacheDirty = false; // Mark as clean after updating vertices
        }

        std::vector<glm::vec3> CubeCollider::getTransformedVertices() {
            if (m_cacheDirty) {
                updateTransformedVertices(); // Ensure vertices are updated if dirty
            }
            return m_transformedVertices;
        }


    } // PHYSICS
} // SGE