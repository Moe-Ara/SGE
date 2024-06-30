//
// Created by Mohamad on 12/06/2024.
//

#include "jump_command.h"

namespace SGE {
    namespace Input {
        void jump_command::run(actors::Actor &actor, float deltaTime) {
            actor.jump(deltaTime);
        }

        bool jump_command::isRunnable() {
            //LATER CHECK FOR GROUND
            return true;
        }

        jump_command::~jump_command() {

        }
    } // Input
} // SGE