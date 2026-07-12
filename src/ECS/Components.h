#ifndef SGE_ECS_COMPONENTS_H
#define SGE_ECS_COMPONENTS_H

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <string>
#include <cstdint>
#include "../GameObjects/Model.h"
#include "../Utils/Material.h"
#include "../Utils/Light.h"
#include "../Graphics/Camera.h"
#include "../Graphics/Texture.h"

namespace SGE::ECS {

    struct SceneIdentityComponent {
        std::uint64_t id{0};
    };

    struct TransformComponent {
        glm::vec3 translation{0.0f, 0.0f, 0.0f};
        glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
        glm::vec3 scale{1.0f, 1.0f, 1.0f};

        [[nodiscard]] glm::mat4 mat4() const {
            glm::mat4 m(1.0f);
            m = glm::translate(m, translation);
            m *= glm::mat4_cast(rotation);
            m = glm::scale(m, scale);
            return m;
        }

        [[nodiscard]] glm::mat3 normalMatrix() const {
            const glm::mat4 m = mat4();
            if (glm::determinant(glm::mat3(m)) == 0.0f) {
                return glm::mat3(1.0f);
            }
            return glm::mat3(glm::transpose(glm::inverse(m)));
        }

        void translateBy(const glm::vec3& delta) { translation += delta; }
        void scaleBy(const glm::vec3& delta) { scale *= delta; }
        void rotateBy(const glm::quat& delta) { rotation = glm::normalize(delta * rotation); }
    };

    struct MeshComponent {
        std::shared_ptr<GAMEOBJECTS::Model> model;
        std::string assetId;
    };

    struct MaterialComponent {
        glm::vec3 albedo{1.0f, 1.0f, 1.0f};
        float metallic{0.0f};
        float roughness{0.5f};
        float ao{1.0f};
        std::shared_ptr<GRAPHICS::Texture> albedoTexture; // optional; overrides albedo when set
        std::string albedoTextureAssetId;
        UTILS::Material phong{};
    };

    struct RigidBodyComponent {
        glm::vec3 velocity{0.0f, 0.0f, 0.0f};
        glm::vec3 acceleration{0.0f, 0.0f, 0.0f};
        glm::vec3 force{0.0f, 0.0f, 0.0f};
        float mass{1.0f};
        float restitution{0.8f};
        bool isStatic{false};
        bool enabled{true};

        void addForce(const glm::vec3& f) { force += f; }
        void clearForces() { force = glm::vec3(0.0f); }
    };

    struct SphereColliderComponent {
        float radius{0.5f};
    };

    struct LightComponent {
        ::PointLight light{};
    };

    struct CameraComponent {
        GRAPHICS::Camera camera{};
        bool isPrimary{false};
        float fovYRadians{glm::radians(45.0f)};
        float nearPlane{0.1f};
        float farPlane{100.0f};
    };

    struct ThirdPersonFollowComponent {
        entt::entity target{entt::null};
        glm::vec3 offset{10.0f, 2.0f, 0.0f};
        float yaw{0.0f};
        float pitch{0.0f};
        float mouseSensitivity{0.05f};
        double lastMouseX{0.0};
        double lastMouseY{0.0};
        bool initialized{false};
        bool mouseLookActive{false};
        bool orbitInitialized{false};
        bool enabled{true};
    };

    struct FreeCameraComponent {
        float movementSpeed{5.0f};
        float sprintMultiplier{2.0f};
        float mouseSensitivity{0.1f};
        float rotationResponsiveness{18.0f};
        float yaw{-90.0f};
        float pitch{0.0f};
        float smoothedYaw{-90.0f};
        float smoothedPitch{0.0f};
        double lastMouseX{0.0};
        double lastMouseY{0.0};
        bool initialized{false};
        bool rotationInitialized{false};
        bool mouseLookActive{false};
        bool enabled{true};
    };

    struct PlayerControllerComponent {
        float movementSpeed{5.0f};
        float jumpSpeed{10.0f};
        bool grounded{false};
    };

    struct TagComponent {
        std::string name;
    };

} // namespace SGE::ECS

#endif // SGE_ECS_COMPONENTS_H
