#ifndef SGE_EDITOR_GIZMOSYSTEM_H
#define SGE_EDITOR_GIZMOSYSTEM_H

#include <entt/entt.hpp>
#include "../Core/EngineMode.h"

namespace SGE::EDITOR {

    class GizmoSystem {
    public:
        enum class Operation {
            Translate,
            Rotate,
            Scale
        };

        void draw(entt::registry& registry, entt::entity selected, CORE::EngineMode mode);

    private:
        Operation operation{Operation::Translate};
        bool localSpace{false};
        bool visible{true};

        void drawControls();
    };

} // namespace SGE::EDITOR

#endif // SGE_EDITOR_GIZMOSYSTEM_H
