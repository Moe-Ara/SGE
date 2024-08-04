//
// Created by Mohamad on 13/07/2024.
//

#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "CapsuleCollider.h"
#include "Collisions.h"

namespace SGE::PHYSICS {
    CapsuleCollider::CapsuleCollider() {

    }

    bool SGE::PHYSICS::CapsuleCollider::checkCollision(SGE::PHYSICS::Collider &other) {
        try {

            switch (other.getType()) {
                case Sphere:
                    return Collisions::intersectsSphereWithCapsule(dynamic_cast<SphereCollider &>(other), *this);
                case Cube:
                    return Collisions::intersectsCubeWithCapsule(dynamic_cast<CubeCollider &>(other), *this);
                case Capsule:
                    return Collisions::intersectsCapsuleWithCapsule(dynamic_cast<CapsuleCollider &>(other), *this);
                case Plane:
                    return Collisions::intersectsPlaneWithCapsule(dynamic_cast<PlaneCollider &>(other), *this);
                default:
                    return false;
            }
        } catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
        }


        return false;
    }
//    void CapsuleCollider::projectCapsuleOnAxis(const glm::vec3 &capsuleStart, const glm::vec3 &capsuleEnd, float capsuleRadius, const glm::vec3 &axis, float &min, float &max) {
//        glm::vec3 dir = glm::normalize(capsuleEnd - capsuleStart);
//        glm::vec3 radialDir = glm::cross(dir, axis);
//        glm::vec3 radialOffset = radialDir * capsuleRadius;
//
//        glm::vec3 capsuleExtremity1 = capsuleStart + radialOffset;
//        glm::vec3 capsuleExtremity2 = capsuleEnd + radialOffset;
//        glm::vec3 capsuleExtremity3 = capsuleStart - radialOffset;
//        glm::vec3 capsuleExtremity4 = capsuleEnd - radialOffset;
//
//        min = std::min({ glm::dot(capsuleExtremity1, axis), glm::dot(capsuleExtremity2, axis), glm::dot(capsuleExtremity3, axis), glm::dot(capsuleExtremity4, axis) });
//        max = std::max({ glm::dot(capsuleExtremity1, axis), glm::dot(capsuleExtremity2, axis), glm::dot(capsuleExtremity3, axis), glm::dot(capsuleExtremity4, axis) });
//    }

    void SGE::PHYSICS::CapsuleCollider::update(const SGE::GAMEOBJECTS::Transform &position) {

        m_transform = position;
    }

    void SGE::PHYSICS::CapsuleCollider::render() const {
        if (m_model) {
            glPushMatrix();

            // Apply the transform
            glm::mat4 mat = m_transform.mat4();
//            glMultMatrixf(&mat[0][0]);
            glMultMatrixf(glm::value_ptr(mat));
            // Apply the scale for height and radius
            glScalef(m_radius, m_height, m_radius);

            // Render the model
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            m_model->bind();
            m_model->render();
            m_model->unbind();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            glPopMatrix();
        }
    }

    const SGE::PHYSICS::ColliderType SGE::PHYSICS::CapsuleCollider::getType() {
        return Capsule;
    }

    SGE::GAMEOBJECTS::Transform SGE::PHYSICS::CapsuleCollider::getTransform() const {
        return m_transform;
    }

    std::shared_ptr<SGE::PHYSICS::BoundingVolume> SGE::PHYSICS::CapsuleCollider::getOBB() const {
        return nullptr;
    }

    float SGE::PHYSICS::CapsuleCollider::calculateRadius() const {
        return 0;
    }


    void CapsuleCollider::setMThreshold(float mThreshold) {

    }

    float CapsuleCollider::getMThreshold() const {
        return 0;
    }

    float CapsuleCollider::getMRadius() const {
        return m_radius;
    }

    float CapsuleCollider::getMHeight() const {
        return m_height;
    }

    void CapsuleCollider::projectCapsuleOnAxis(const glm::vec3 &capsuleStart, const glm::vec3 &capsuleEnd,
                                               float capsuleRadius, const glm::vec3 &axis, float &min, float &max) {
        glm::vec3 dir = glm::normalize(capsuleEnd - capsuleStart);
        glm::vec3 radialDir = glm::cross(dir, axis);
        glm::vec3 radialOffset = radialDir * capsuleRadius;

        glm::vec3 capsuleExtremity1 = capsuleStart + radialOffset;
        glm::vec3 capsuleExtremity2 = capsuleEnd + radialOffset;
        glm::vec3 capsuleExtremity3 = capsuleStart - radialOffset;
        glm::vec3 capsuleExtremity4 = capsuleEnd - radialOffset;

        min = std::min({ glm::dot(capsuleExtremity1, axis), glm::dot(capsuleExtremity2, axis), glm::dot(capsuleExtremity3, axis), glm::dot(capsuleExtremity4, axis) });
        max = std::max({ glm::dot(capsuleExtremity1, axis), glm::dot(capsuleExtremity2, axis), glm::dot(capsuleExtremity3, axis), glm::dot(capsuleExtremity4, axis) });

    }


}