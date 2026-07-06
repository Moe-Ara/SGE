//
// Created by Developer on [date].
//

#ifndef GLCPP_IEVENT_H
#define GLCPP_IEVENT_H

#include <string>

namespace SGE::EVENTS {
    class IEvent {
    public:
        virtual ~IEvent() = default;
        
        // Get event type
        virtual std::string getType() const = 0;
        
        // Get event name for debugging
        virtual std::string getName() const = 0;
    };
};

#endif // GLCPP_IEVENT_H