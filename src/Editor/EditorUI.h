#ifndef SGE_EDITOR_EDITORUI_H
#define SGE_EDITOR_EDITORUI_H

#include <entt/entt.hpp>
#include "../Core/EngineMode.h"

struct GLFWwindow;

namespace SGE::EDITOR {

    // A minimal in-process Dear ImGui debug/editor overlay: a Play/Stop toolbar,
    // an entity list, and a live component inspector for whichever entity is
    // selected.
    class EditorUI {
    public:
        explicit EditorUI(GLFWwindow* window, const char* glslVersion = "#version 330");
        ~EditorUI();

        EditorUI(const EditorUI&) = delete;
        EditorUI& operator=(const EditorUI&) = delete;

        void beginFrame();
        void draw(entt::registry& registry, SGE::CORE::EngineMode mode);
        void endFrame();

        // True at most once per click: Application owns the mode, so the
        // toolbar just raises a request and clears it once read.
        bool consumePlayToggleRequest();

    private:
        entt::entity selected{entt::null};
        bool playToggleRequested{false};

        void drawToolbar(SGE::CORE::EngineMode mode);
        void drawEntityList(entt::registry& registry);
        void drawInspector(entt::registry& registry);
    };

} // namespace SGE::EDITOR

#endif // SGE_EDITOR_EDITORUI_H
