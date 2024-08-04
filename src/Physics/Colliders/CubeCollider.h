//
// Created by Mohamad on 13/07/2024.
//

#ifndef GLCPP_CUBECOLLIDER_H
#define GLCPP_CUBECOLLIDER_H


#include <memory>
#include "../Collider.h"
#include "../../GameObjects/Model.h"
#include "../../Utils/ModelLoader.h"
#include "SphereCollider.h"
#include <cfloat>

namespace SGE::PHYSICS {

        class CubeCollider :public Collider{
        public:
            CubeCollider();
            virtual ~CubeCollider();
            bool checkCollision( Collider &other) override;

            void update(const SGE::GAMEOBJECTS::Transform &position) override;

            void render() const override;

            const ColliderType getType() override;
            [[nodiscard]] GAMEOBJECTS::Transform getTransform() const override;
            [[nodiscard]] std::shared_ptr<BoundingVolume> getOBB() const override;


            [[nodiscard]] glm::vec3 calculateMin() const;

            [[nodiscard]] glm::vec3 calculateMax() const;

            glm::vec3 calculateLocalMin() const;

            glm::vec3 calculateLocalMax() const;

            void projectCube(const glm::vec3 &axis, float &min, float &max);

            [[nodiscard]] std::vector<glm::vec3> getTransformedVertices();

        private:
            SGE::GAMEOBJECTS::Transform m_transform;
            std::shared_ptr<SGE::GAMEOBJECTS::Model> m_model{SGE::UTILS::ModelLoader::loadModelFromFile("cube.obj")};

            mutable std::vector<glm::vec3>  m_transformedVertices;
            mutable bool m_cacheDirty;
            void updateTransformedVertices() const;

        };

    } // PHYSICS
// SGE

#endif //GLCPP_CUBECOLLIDER_H
