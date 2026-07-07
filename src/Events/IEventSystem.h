//
// Created by Developer on [date].
//

#ifndef GLCPP_IEVENTSYSTEM_H
#define GLCPP_IEVENTSYSTEM_H

#include "IEvent.h"
#include <cstddef>
#include <functional>
#include <string>

namespace SGE::EVENTS {
    using SubscriptionId = std::size_t;

    class IEventSystem {
    public:
        virtual ~IEventSystem() = default;

        // Subscribe to an event type, returns a token that identifies this subscription
        virtual SubscriptionId subscribe(const std::string& eventType,
                              std::function<void(const IEvent&)> handler) = 0;

        // Unsubscribe a single subscription previously returned by subscribe()
        virtual void unsubscribe(const std::string& eventType, SubscriptionId id) = 0;

        // Publish an event
        virtual void publish(const IEvent& event) = 0;

        // Get event system name
        virtual std::string getName() const = 0;
    };
};

#endif // GLCPP_IEVENTSYSTEM_H
