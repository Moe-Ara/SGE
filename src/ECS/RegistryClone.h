#ifndef SGE_ECS_REGISTRYCLONE_H
#define SGE_ECS_REGISTRYCLONE_H

#include "SceneSerializer.h"

namespace SGE::ECS {

    [[nodiscard]] inline entt::registry cloneRegistry(entt::registry& source) {
        return SceneSerializer::cloneRuntime(source);
    }

} // namespace SGE::ECS

#endif // SGE_ECS_REGISTRYCLONE_H
