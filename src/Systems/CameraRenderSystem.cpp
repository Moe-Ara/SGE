#include "CameraRenderSystem.h"
#include "../ECS/Components.h"
#include "../Graphics/Window.h"
#include "../Utils/CameraMath.h"

namespace SGE::SYSTEMS {

    CameraRenderSystem::CameraRenderSystem(GRAPHICS::Window& window) : window(window) {}

    void CameraRenderSystem::update(entt::registry& registry, float deltaTime) {
        (void)deltaTime;

        if (window.isHeadless()) {
            return;
        }

        updateProjection(registry);
        updateView(registry);
    }

    void CameraRenderSystem::updateProjection(entt::registry& registry) const {
        const float aspect = window.getHeight() > 0
            ? static_cast<float>(window.getWidth()) / static_cast<float>(window.getHeight())
            : 16.0f / 9.0f;

        auto view = registry.view<ECS::CameraComponent>();

        for (auto entity : view) {
            auto& cam = view.get<ECS::CameraComponent>(entity);
            cam.camera.setPrespectiveProjection(
                cam.fovYRadians,
                aspect,
                cam.nearPlane,
                cam.farPlane
            );
        }
    }

    void CameraRenderSystem::updateView(entt::registry& registry) const {
        auto view = registry.view<ECS::CameraComponent, ECS::TransformComponent>();

        for (auto entity : view) {
            auto& cam = view.get<ECS::CameraComponent>(entity);
            const auto& transform = view.get<ECS::TransformComponent>(entity);

            cam.camera.setViewDirection(
                transform.translation,
                UTILS::cameraForward(transform.rotation),
                UTILS::worldUp()
            );
        }
    }

} // namespace SGE::SYSTEMS
