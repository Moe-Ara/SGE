//
// Created by Mohamad on 12/07/2024.
//

#ifndef GLCPP_PHYSICSENGINE_H
#define GLCPP_PHYSICSENGINE_H

#include "../Graphics/Scene.h"
namespace SGE {
    namespace PHYSICS {

        class PhysicsEngine {
        public:

            virtual ~PhysicsEngine();
            static void update(const SGE::GRAPHICS::Scene& scene, float deltaTime);
        private:
            PhysicsEngine();
            constexpr static const glm::vec3 GRAVITY_FORCE{0.0f, 9.8f, 0.0f};

        };

    } // PHYSICS
} // SGE

#endif //GLCPP_PHYSICSENGINE_H
