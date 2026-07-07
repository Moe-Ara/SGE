//
// Created by Developer on [date].
//

#ifndef GLCPP_EVENTSYSTEM_H
#define GLCPP_EVENTSYSTEM_H

#include "IEventSystem.h"
#include "Event.h"
#include <unordered_map>
#include <vector>
#include <functional>
#include <mutex>
#include <utility>

namespace SGE::EVENTS {
    class EventSystem : public IEventSystem {
    private:
        using Subscriber = std::pair<SubscriptionId, std::function<void(const IEvent&)>>;
        std::unordered_map<std::string, std::vector<Subscriber>> subscribers;
        std::string systemName;
        SubscriptionId nextId;
        mutable std::mutex mutex;

    public:
        EventSystem(const std::string& name = "DefaultEventSystem");
        ~EventSystem() override = default;

        // IEventSystem interface
        SubscriptionId subscribe(const std::string& eventType,
                      std::function<void(const IEvent&)> handler) override;
        void unsubscribe(const std::string& eventType, SubscriptionId id) override;
        void publish(const IEvent& event) override;
        std::string getName() const override;

        // Event system specific methods
        void clearSubscribers();
    };
};

#endif // GLCPP_EVENTSYSTEM_H
