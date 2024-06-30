//
// Created by Mohamad on 12/06/2024.
//

#ifndef GLCPP_JUMP_COMMAND_H
#define GLCPP_JUMP_COMMAND_H

#include "command.h"

namespace SGE {
    namespace Input {

        class jump_command: command {
        public:
            void run(actors::Actor& actor, float deltaTime) override;
            bool isRunnable() override;
            ~jump_command() override;
        };

    } // Input
} // SGE

#endif //GLCPP_JUMP_COMMAND_H
