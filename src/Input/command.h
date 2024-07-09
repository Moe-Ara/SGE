//
// Created by Mohamad on 12/06/2024.
//

#ifndef GLCPP_COMMAND_H
#define GLCPP_COMMAND_H

#include "../GameObjects/Actor.h"
namespace SGE::Input{
    class command{
    public:
        virtual void run(actors::Actor& actor, float deltaTime)=0;
        virtual bool isRunnable()=0;
        virtual ~command(){};
    };
}

#endif //GLCPP_COMMAND_H
