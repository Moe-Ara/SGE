#include <glm/gtx/string_cast.hpp>
#include "SphereCollider.h"
#include "Collisions.h"
namespace SGE::PHYSICS {

    SphereCollider::SphereCollider() {
        m_radius = calculateRadius();
    }

    bool SphereCollider::checkCollision(Collider& other) {
        try{
            switch (other.getType()) {
                case Sphere:
                    return Collisions::intersectsSphereWithSphere(*this,dynamic_cast<SphereCollider&>(other));
                case Cube:
                    return Collisions::intersectsSphereWithCube(*this,dynamic_cast<CubeCollider&>(other));
                case Plane:
                    return Collisions::intersectsSphereWithPlane(*this,dynamic_cast<PlaneCollider&>(other));
                case Capsule:
                    return Collisions::intersectsSphereWithCapsule(*this,dynamic_cast<CapsuleCollider&>(other));
                default:
                    return false;
            }
        } catch (std::exception &e){
            std::cerr<< e.what()<< std::endl;
        }

    }

    void SphereCollider::update(const SGE::GAMEOBJECTS::Transform &position) {
        m_transform = position;
        m_radius = calculateRadius(); // Recalculate radius if transform changes
    }

    void SphereCollider::render() const {
        if (m_model) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            m_model->bind();
            m_model->render();
            m_model->unbind();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


        }
    }

    std::shared_ptr<SGE::PHYSICS::BoundingVolume> SphereCollider::getOBB() const {
        return nullptr;
        glm::vec3 extents(m_radius, m_radius, m_radius);
        return std::make_shared<OrientedBoundingBoxes>(m_transform, extents, glm::mat3(1.0f));
    }

    const ColliderType SphereCollider::getType() {
        return Sphere;
    }

    SGE::GAMEOBJECTS::Transform SphereCollider::getTransform() const {
        return m_transform;
    }

    float SphereCollider::getMRadius() const {
        return m_radius;
    }

    float SphereCollider::getMThreshold() const {
        return m_threshold;
    }

    void SphereCollider::setMThreshold(float mThreshold) {
        m_threshold = mThreshold;
    }

    float SphereCollider::calculateRadius() const {
        if (!m_model) {
            std::cerr << "Error: Model not initialized!" << std::endl;
            return 0.0f;
        }

        const auto &vertices = m_model->getVertices();
        if (vertices.empty()) {
            std::cerr << "Error: Model has no vertices!" << std::endl;
            return 0.0f;
        }

        float maxDistance = 0.0f;
        glm::vec3 scale = m_transform.getMScale();

        for (const auto &vertex : vertices) {
            glm::vec3 scaledPosition = vertex.position * scale;
            float distance = glm::length(scaledPosition);
            if (distance > maxDistance) {
                maxDistance = distance;
            }
        }

        return maxDistance;
    }

}
