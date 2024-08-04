//
// Created by Mohamad on 13/07/2024.
//

#ifndef GLCPP_FLOOR_H
#define GLCPP_FLOOR_H

#include <memory>

#include "../GameObjects/GameObject.h"
#include "../Physics/Colliders/OBBCollider.h"
#include "../Physics/Colliders/PlaneCollider.h"

namespace SGE::DEBUGGING {

class Floor : public SGE::GAMEOBJECTS::GameObject{
public:
    Floor();
    virtual ~Floor() override;
    [[nodiscard]] glm::vec3 getColor() const override;
    std::shared_ptr<SGE::PHYSICS::Collider> getCollider() override;

private:
    std::shared_ptr<SGE::PHYSICS::PlaneCollider> m_collider;

        };

    } // DEBUGGING
// SGE

#endif //GLCPP_FLOOR_H
