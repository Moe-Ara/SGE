//#include <glm/gtc/quaternion.hpp>
//#include <glm/gtx/string_cast.hpp>
//#include "OBBCollider.h"
//
//namespace SGE::PHYSICS {
//
//    OBBCollider::OBBCollider(std::shared_ptr<SGE::GAMEOBJECTS::Model> model)
//            : m_model(std::move(model)) {
//        calculateHalfSizesAndCenterFromModel();
//        m_obb = std::make_shared<OrientedBoundingBoxes>(m_center, m_halfSizes, glm::mat3(1.0f));
//    }
//
//    bool OBBCollider::checkCollision(const Collider &other) {
//        return m_obb->intersects(*other.getOBB());
//    }
//
//    void OBBCollider::update(const GAMEOBJECTS::Transform &m_transform) {
//        auto oldScale = m_transform.getMScale();
//        m_transform = m_transform;
//
//        if (oldScale != m_transform.getMScale()) {
//            calculateHalfSizesAndCenterFromModel();
//        }
//
//        glm::mat3 orientation = glm::mat3_cast(m_transform.getMRotation());
//        m_obb = std::make_shared<OrientedBoundingBoxes>(m_transform.getMTranslation() + m_center, m_halfSizes, orientation);
//    }
//
//    void OBBCollider::render() const {
//        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), m_transform.getMTranslation() + m_center) *
//                                glm::mat4_cast(m_transform.getMRotation()) *
//                                glm::scale(glm::mat4(1.0f), m_halfSizes);
//
//        if (m_model) {
//            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//            m_model->bind();
//            glDrawElements(GL_LINES, m_model->getMIndexCount(), GL_UNSIGNED_INT, nullptr);
//            m_model->unbind();
//            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//        }
//    }
//
//    void OBBCollider::render(std::shared_ptr<SGE::GRAPHICS::Shader> &shader) const {
//        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), m_transform.getMTranslation() + m_center) *
//                                glm::mat4_cast(m_transform.getMRotation()) *
//                                glm::scale(glm::mat4(1.0f), m_halfSizes);
//
//        shader->setUniformMat4("m_transform", modelMatrix);
//
//        if (m_model) {
//            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//            m_model->bind();
//            glDrawElements(GL_LINES, m_model->getMIndexCount(), GL_UNSIGNED_INT, nullptr);
//            m_model->unbind();
//            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//        }
//    }
//
//    std::shared_ptr<BoundingVolume> OBBCollider::getOBB() const {
//        return m_obb;
//    }
//
//    void OBBCollider::calculateHalfSizesAndCenterFromModel() {
//        glm::vec3 minExtent(FLT_MAX);
//        glm::vec3 maxExtent(-FLT_MAX);
//        const auto& vertices = m_model->getVertices();
//
//        if (vertices.empty()) {
//            std::cerr << "Error: Model has no vertices!" << std::endl;
//            return;
//        }
//
//        for (const auto& vertex : vertices) {
//            minExtent = glm::min(minExtent, vertex.position);
//            maxExtent = glm::max(maxExtent, vertex.position);
//        }
//
//        glm::vec3 localHalfSizes = (maxExtent - minExtent) / 2.0f;
//        glm::vec3 scale = m_transform.getMScale();
//        m_halfSizes = localHalfSizes ;// * scale;
//
//        // Calculate the center of the OBB
//        m_center = (minExtent + maxExtent) / 2.0f;
//    }
//
//    GAMEOBJECTS::Transform OBBCollider::getTransform() const {
//        return m_transform;
//    }
//
//    const ColliderType OBBCollider::getType() {
//        return OBB;
//    }
//
//}
