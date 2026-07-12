#ifndef SGE_ECS_QUERIES_H
#define SGE_ECS_QUERIES_H

#include <entt/entt.hpp>
#include "Components.h"
#include <stdexcept>

namespace SGE::ECS {

    // Single source of truth for "which camera is active" so every system that
    // needs it (InputSystem for movement basis, RenderSystem for view/projection,
    // ...) is guaranteed to agree, even if zero or more than one CameraComponent
    // is marked isPrimary.
    inline entt::entity findPrimaryCamera(entt::registry& registry) {
        auto view = registry.view<CameraComponent>();
        entt::entity primary = entt::null;
        for (auto entity : view) {
            if (view.get<CameraComponent>(entity).isPrimary) {
                if (primary != entt::null) {
                    throw std::logic_error("Scene contains more than one primary camera");
                }
                primary = entity;
            }
        }
        return primary;
    }

    inline void setPrimaryCamera(entt::registry& registry, entt::entity selected) {
        if (!registry.valid(selected) || !registry.all_of<CameraComponent>(selected)) {
            throw std::invalid_argument("Primary camera must be a valid camera entity");
        }
        auto view = registry.view<CameraComponent>();
        for (auto entity : view) {
            view.get<CameraComponent>(entity).isPrimary = entity == selected;
        }
    }

} // namespace SGE::ECS

#endif // SGE_ECS_QUERIES_H
