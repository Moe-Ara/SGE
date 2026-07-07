#ifndef SGE_EDITOR_EDITORUI_H
#define SGE_EDITOR_EDITORUI_H

#include <entt/entt.hpp>

struct GLFWwindow;

namespace SGE::EDITOR {

    // A minimal in-process Dear ImGui debug/editor overlay: an entity list and a
    // live component inspector for whichever entity is selected.
    class EditorUI {
    public:
        explicit EditorUI(GLFWwindow* window, const char* glslVersion = "#version 330");
        ~EditorUI();

        EditorUI(const EditorUI&) = delete;
        EditorUI& operator=(const EditorUI&) = delete;

        void beginFrame();
        void draw(entt::registry& registry);
        void endFrame();

    private:
        entt::entity selected{entt::null};

        void drawEntityList(entt::registry& registry);
        void drawInspector(entt::registry& registry);
    };

} // namespace SGE::EDITOR

#endif // SGE_EDITOR_EDITORUI_H
