#include "PlaneCollider.h"
#include "SphereCollider.h"
#include "Collisions.h"
namespace SGE::PHYSICS {
    bool SGE::PHYSICS::PlaneCollider::checkCollision(SGE::PHYSICS::Collider &other) {
        switch (other.getType()) {
            case Sphere:
                return Collisions::intersectsSphereWithPlane(dynamic_cast<SphereCollider&>(other),*this);
            case Cube:
                return Collisions::intersectsCubeWithPlane(dynamic_cast<CubeCollider&>(other),*this);
            case Plane:
                return Collisions::intersectsPlaneWithPlane(dynamic_cast<PlaneCollider&>(other),*this);
            case Capsule:
//                return intersectsWithCapsule(other);
            default:
                return false;
        }
    }

    void SGE::PHYSICS::PlaneCollider::update(const SGE::GAMEOBJECTS::Transform &position) {
        m_transform = position;
        m_normal = glm::normalize(position.normalMatrix() * glm::vec3(0.f, -1.f, 0.f));
    }

    void SGE::PHYSICS::PlaneCollider::render() const {
        glm::vec3 point = m_transform.getMTranslation();
        glm::vec3 tangent1 = glm::cross(m_normal, glm::vec3(1.0f, 0.0f, 0.0f));
        if (glm::length(tangent1) < 0.1f) {
            tangent1 = glm::cross(m_normal, glm::vec3(0.0f, 1.0f, 0.0f));
        }
        tangent1 = glm::normalize(tangent1);
        glm::vec3 tangent2 = glm::cross(m_normal, tangent1);

        float planeSize = 100.0f;

        glBegin(GL_LINES);
        glColor3f(1.0f, 0.0f, 0.0f); // Color red for plane lines

        for (float i = -planeSize; i <= planeSize; i += 1.0f) {
            glm::vec3 start = point + tangent1 * i + tangent2 * -planeSize;
            glm::vec3 end = point + tangent1 * i + tangent2 * planeSize;
            glVertex3fv(&start.x);
            glVertex3fv(&end.x);

            start = point + tangent2 * i + tangent1 * -planeSize;
            end = point + tangent2 * i + tangent1 * planeSize;
            glVertex3fv(&start.x);
            glVertex3fv(&end.x);
        }

        glEnd();
    }

    const SGE::PHYSICS::ColliderType SGE::PHYSICS::PlaneCollider::getType() {
        return Plane;
    }

    SGE::GAMEOBJECTS::Transform SGE::PHYSICS::PlaneCollider::getTransform() const {
        return m_transform;
    }

    std::shared_ptr<SGE::PHYSICS::BoundingVolume> SGE::PHYSICS::PlaneCollider::getOBB() const {
        return nullptr;
    }

    const glm::vec3 &PlaneCollider::getMNormal() const {
        return m_normal;
    }

    float PlaneCollider::getMDistance() const {
        return m_distance;
    }


}
