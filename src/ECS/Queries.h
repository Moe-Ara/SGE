#ifndef SGE_ECS_QUERIES_H
#define SGE_ECS_QUERIES_H

#include <entt/entt.hpp>
#include "Components.h"

namespace SGE::ECS {

    // Single source of truth for "which camera is active" so every system that
    // needs it (InputSystem for movement basis, RenderSystem for view/projection,
    // ...) is guaranteed to agree, even if zero or more than one CameraComponent
    // is marked isPrimary.
    inline entt::entity findPrimaryCamera(entt::registry& registry) {
        auto view = registry.view<CameraComponent>();
        for (auto entity : view) {
            if (view.get<CameraComponent>(entity).isPrimary) {
                return entity;
            }
        }
        return entt::null;
    }

} // namespace SGE::ECS

#endif // SGE_ECS_QUERIES_H
