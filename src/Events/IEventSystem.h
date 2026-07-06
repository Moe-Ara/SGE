//
// Created by Developer on [date].
//

#ifndef GLCPP_IEVENTSYSTEM_H
#define GLCPP_IEVENTSYSTEM_H

#include "IEvent.h"
#include <functional>
#include <string>

namespace SGE::EVENTS {
    class IEventSystem {
    public:
        virtual ~IEventSystem() = default;
        
        // Subscribe to an event type
        virtual void subscribe(const std::string& eventType, 
                              std::function<void(const IEvent&)> handler) = 0;
        
        // Unsubscribe from an event type
        virtual void unsubscribe(const std::string& eventType) = 0;
        
        // Publish an event
        virtual void publish(const IEvent& event) = 0;
        
        // Get event system name
        virtual std::string getName() const = 0;
    };
};

#endif // GLCPP_IEVENTSYSTEM_H