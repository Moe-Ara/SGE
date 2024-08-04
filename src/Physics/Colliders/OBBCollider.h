////
//// Created by Mohamad on 15/07/2024.
////
//
//#ifndef GLCPP_OBBCOLLIDER_H
//#define GLCPP_OBBCOLLIDER_H
//
//
//#include "../Collider.h"
//#include "../BoundingVolumes/OrientedBoundingBoxes.h"
//#include "../../GameObjects/Model.h"
//#include "../../Utils/ModelLoader.h"
//#include "../../Graphics/Shader.h"
//#include <cfloat>
//#include <memory>
//namespace SGE::PHYSICS {
//
//
//        class OBBCollider : public Collider {
//        public:
//            explicit OBBCollider(std::shared_ptr<SGE::GAMEOBJECTS::Model> model);
//
//            ~OBBCollider() override = default;
//
//            const ColliderType getType() override;
//
//            GAMEOBJECTS::Transform getTransform() const override;
//
//            bool checkCollision(const Collider &other) override;
//            void update(const GAMEOBJECTS::Transform &m_transform) override;
//            void render() const override;
//            void render(std::shared_ptr<SGE::GRAPHICS::Shader> &shader) const;
//            std::shared_ptr<BoundingVolume> getOBB() const override;
//
//        private:
//            void calculateHalfSizesAndCenterFromModel();
//
//            SGE::GAMEOBJECTS::Transform m_transform;
//            std::shared_ptr<SGE::GAMEOBJECTS::Model> m_model;
//            glm::vec3 m_halfSizes;
//            glm::vec3 m_center;
//            std::shared_ptr<SGE::PHYSICS::OrientedBoundingBoxes> m_obb;
//
//        };
//    }
//
//// SGE
//
//#endif //GLCPP_OBBCOLLIDER_H
