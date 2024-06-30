//
// Created by Mohamad on 12/06/2024.
//

#ifndef GLCPP_ACTOR_H
#define GLCPP_ACTOR_H

#include "GameObject.h"

namespace SGE::actors{
    class Actor {

    public:
        virtual void jump(float deltaTime)=0;
        virtual void move(glm::vec3 movement,float deltaTime)=0;
        virtual void fire(float deltaTime)=0;
        virtual void dash(float deltaTime)=0;
        virtual ~Actor(){};
        virtual Transform getTransform()=0;
        virtual void update(float deltaTime)=0;
    };
}



#endif //GLCPP_ACTOR_H
