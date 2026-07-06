//
// Created by Developer on [date].
//

#ifndef GLCPP_EVENT_H
#define GLCPP_EVENT_H

#include "IEvent.h"
#include <string>

namespace SGE::EVENTS {
    class Event : public IEvent {
    private:
        std::string eventType;
        std::string eventName;
        
    public:
        Event(const std::string& type, const std::string& name);
        ~Event() override = default;
        
        // IEvent interface
        std::string getType() const override;
        std::string getName() const override;
    };
};

#endif // GLCPP_EVENT_H