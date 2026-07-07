#include "EditorUI.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../ECS/Components.h"
#include <string>

namespace SGE::EDITOR {

    EditorUI::EditorUI(GLFWwindow* window, const char* glslVersion) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glslVersion);
    }

    EditorUI::~EditorUI() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void EditorUI::beginFrame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void EditorUI::draw(entt::registry& registry) {
        ImGui::Begin("Scene");
        drawEntityList(registry);
        ImGui::End();

        ImGui::Begin("Inspector");
        drawInspector(registry);
        ImGui::End();
    }

    void EditorUI::endFrame() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void EditorUI::drawEntityList(entt::registry& registry) {
        auto view = registry.view<ECS::TagComponent>();
        for (auto entity : view) {
            const auto& tag = view.get<ECS::TagComponent>(entity);
            const std::string label = (tag.name.empty() ? std::string("Entity") : tag.name) +
                                       "##" + std::to_string(static_cast<uint32_t>(entity));
            const bool isSelected = (entity == selected);
            if (ImGui::Selectable(label.c_str(), isSelected)) {
                selected = entity;
            }
        }
    }

    void EditorUI::drawInspector(entt::registry& registry) {
        if (selected == entt::null || !registry.valid(selected)) {
            ImGui::TextUnformatted("No entity selected");
            return;
        }

        if (auto* tag = registry.try_get<ECS::TagComponent>(selected)) {
            ImGui::Text("Tag: %s", tag->name.c_str());
        }

        if (auto* transform = registry.try_get<ECS::TransformComponent>(selected)) {
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::DragFloat3("Translation", &transform->translation.x, 0.1f);
                ImGui::DragFloat3("Scale", &transform->scale.x, 0.05f, 0.01f, 100.0f);
            }
        }

        if (auto* material = registry.try_get<ECS::MaterialComponent>(selected)) {
            if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::ColorEdit3("Albedo", &material->albedo.x);
                ImGui::SliderFloat("Metallic", &material->metallic, 0.0f, 1.0f);
                ImGui::SliderFloat("Roughness", &material->roughness, 0.02f, 1.0f);
                ImGui::SliderFloat("AO", &material->ao, 0.0f, 1.0f);
                if (material->albedoTexture) {
                    ImGui::Text("Albedo Map: bound (overrides Albedo color)");
                    if (ImGui::Button("Clear Albedo Map")) {
                        material->albedoTexture.reset();
                    }
                } else {
                    ImGui::TextDisabled("Albedo Map: none");
                }
            }
        }

        if (auto* body = registry.try_get<ECS::RigidBodyComponent>(selected)) {
            if (ImGui::CollapsingHeader("Rigid Body")) {
                ImGui::DragFloat3("Velocity", &body->velocity.x, 0.1f);
                ImGui::DragFloat("Mass", &body->mass, 0.1f, 0.01f, 1000.0f);
                ImGui::DragFloat("Restitution", &body->restitution, 0.01f, 0.0f, 1.0f);
                ImGui::Checkbox("Static", &body->isStatic);
                ImGui::Checkbox("Enabled", &body->enabled);
            }
        }

        if (auto* light = registry.try_get<ECS::LightComponent>(selected)) {
            if (ImGui::CollapsingHeader("Light")) {
                ImGui::DragFloat3("Position", &light->light.position.x, 0.1f);
                ImGui::ColorEdit3("Color", &light->light.color.x);
                ImGui::DragFloat("Intensity", &light->light.intensity, 0.1f, 0.0f, 100.0f);
            }
        }

        if (auto* collider = registry.try_get<ECS::SphereColliderComponent>(selected)) {
            if (ImGui::CollapsingHeader("Sphere Collider")) {
                ImGui::DragFloat("Radius", &collider->radius, 0.05f, 0.01f, 100.0f);
            }
        }
    }

} // namespace SGE::EDITOR
