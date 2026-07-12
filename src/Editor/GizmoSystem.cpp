#include "GizmoSystem.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "../ECS/Components.h"
#include "../ECS/Queries.h"
#include "../Utils/CameraMath.h"
#include <glm/gtx/matrix_decompose.hpp>
#include <algorithm>

namespace SGE::EDITOR {

    namespace {
        ImGuizmo::OPERATION toImGuizmoOperation(GizmoSystem::Operation operation) {
            switch (operation) {
                case GizmoSystem::Operation::Translate:
                    return ImGuizmo::TRANSLATE;
                case GizmoSystem::Operation::Rotate:
                    return ImGuizmo::ROTATE;
                case GizmoSystem::Operation::Scale:
                    return ImGuizmo::SCALE;
            }
            return ImGuizmo::TRANSLATE;
        }

        bool decomposeTransform(const glm::mat4& matrix, ECS::TransformComponent& transform) {
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::vec3 translation;
            glm::vec3 scale;
            glm::quat rotation;

            if (!glm::decompose(matrix, scale, rotation, translation, skew, perspective)) {
                return false;
            }

            transform.translation = translation;
            transform.rotation = glm::normalize(rotation);
            transform.scale = scale;
            return true;
        }
    }

    void GizmoSystem::draw(entt::registry& registry,
                           entt::entity selected,
                           CORE::EngineMode mode) {
        if (mode != CORE::EngineMode::Inspection) {
            return;
        }

        if (!ImGui::GetIO().WantTextInput && ImGui::IsKeyPressed(ImGuiKey_F3, false)) {
            visible = !visible;
        }
        if (!visible) {
            return;
        }

        const entt::entity cameraEntity = ECS::findPrimaryCamera(registry);
        if (cameraEntity == entt::null) {
            return;
        }

        const auto& io = ImGui::GetIO();
        if (io.DisplaySize.x <= 0.0f || io.DisplaySize.y <= 0.0f) {
            return;
        }

        const auto& camera = registry.get<ECS::CameraComponent>(cameraEntity).camera;
        const float panelTop = 48.0f;
        const float scenePanelWidth = std::clamp(io.DisplaySize.x * 0.23f, 180.0f, 260.0f);
        const float inspectorWidth = std::clamp(io.DisplaySize.x * 0.30f, 240.0f, 340.0f);
        const float viewportWidth = std::max(io.DisplaySize.x - scenePanelWidth - inspectorWidth, 1.0f);
        const float viewportHeight = std::max(io.DisplaySize.y - panelTop, 1.0f);

        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
        ImGuizmo::SetRect(scenePanelWidth, panelTop, viewportWidth, viewportHeight);

        const glm::mat4 worldTransform{1.0f};
        ImGuizmo::DrawGridCustomColor(
            &camera.getViewMatrix()[0][0],
            &camera.getProjectionMatrix()[0][0],
            &worldTransform[0][0],
            100.0f,
            10.0f,
            10u,
            IM_COL32(105, 112, 122, 90),
            IM_COL32(78, 84, 94, 42),
            IM_COL32(150, 156, 165, 125)
        );
        ImGuizmo::DrawAxes(
            &camera.getViewMatrix()[0][0],
            &camera.getProjectionMatrix()[0][0],
            &worldTransform[0][0],
            1
        );

        drawControls();

        if (selected == entt::null || !registry.valid(selected)) {
            return;
        }

        auto* transform = registry.try_get<ECS::TransformComponent>(selected);
        if (transform == nullptr) {
            return;
        }
        glm::mat4 model = transform->mat4();

        const ImGuizmo::MODE gizmoMode =
            operation == Operation::Scale || !localSpace ? ImGuizmo::WORLD : ImGuizmo::LOCAL;

        if (ImGuizmo::Manipulate(&camera.getViewMatrix()[0][0],
                                 &camera.getProjectionMatrix()[0][0],
                                 toImGuizmoOperation(operation),
                                 gizmoMode,
                                 &model[0][0]) &&
            ImGuizmo::IsUsing()) {
            if (decomposeTransform(model, *transform)) {
                if (auto* freeCamera = registry.try_get<ECS::FreeCameraComponent>(selected)) {
                    const glm::vec2 yawPitch =
                        UTILS::yawPitchFromForward(UTILS::cameraForward(transform->rotation));
                    freeCamera->yaw = yawPitch.x;
                    freeCamera->pitch = yawPitch.y;
                }
            }
        }
    }

    void GizmoSystem::drawControls() {
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, 54.0f),
                                ImGuiCond_Always, ImVec2(0.5f, 0.0f));
        ImGui::SetNextWindowBgAlpha(0.85f);
        ImGui::Begin("Gizmo", nullptr,
                     ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);

        if (!ImGui::GetIO().WantTextInput) {
            if (ImGui::IsKeyPressed(ImGuiKey_1)) operation = Operation::Translate;
            if (ImGui::IsKeyPressed(ImGuiKey_2)) operation = Operation::Rotate;
            if (ImGui::IsKeyPressed(ImGuiKey_3)) operation = Operation::Scale;
            if (ImGui::IsKeyPressed(ImGuiKey_L)) localSpace = !localSpace;
        }

        if (ImGui::RadioButton("Move", operation == Operation::Translate)) {
            operation = Operation::Translate;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", operation == Operation::Rotate)) {
            operation = Operation::Rotate;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", operation == Operation::Scale)) {
            operation = Operation::Scale;
        }

        if (operation != Operation::Scale) {
            ImGui::Checkbox("Local", &localSpace);
        }

        ImGui::End();
    }

} // namespace SGE::EDITOR
