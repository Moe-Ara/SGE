//
// Created by Developer on [date].
//

#include "EventSystem.h"
#include <algorithm>

namespace SGE::EVENTS {
    EventSystem::EventSystem(const std::string& name) : systemName(name), nextId(1) {}

    SubscriptionId EventSystem::subscribe(const std::string& eventType,
                               std::function<void(const IEvent&)> handler) {
        std::lock_guard<std::mutex> lock(mutex);
        const SubscriptionId id = nextId++;
        subscribers[eventType].emplace_back(id, std::move(handler));
        return id;
    }

    void EventSystem::unsubscribe(const std::string& eventType, SubscriptionId id) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = subscribers.find(eventType);
        if (it == subscribers.end()) {
            return;
        }
        auto& handlers = it->second;
        handlers.erase(
            std::remove_if(handlers.begin(), handlers.end(),
                            [id](const Subscriber& sub) { return sub.first == id; }),
            handlers.end());
    }
    
    void EventSystem::publish(const IEvent& event) {
        // Copy the handler list out while holding the lock, then invoke callbacks
        // with the lock released. Handlers are allowed to call subscribe/unsubscribe/
        // publish on this same EventSystem (e.g. unsubscribing themselves in
        // response to the event they just handled) without deadlocking on our
        // non-recursive mutex.
        std::vector<std::function<void(const IEvent&)>> handlers;
        {
            std::lock_guard<std::mutex> lock(mutex);
            auto it = subscribers.find(event.getType());
            if (it == subscribers.end()) {
                return;
            }
            handlers.reserve(it->second.size());
            for (const auto& subscriber : it->second) {
                handlers.push_back(subscriber.second);
            }
        }

        for (const auto& handler : handlers) {
            handler(event);
        }
    }
    
    std::string EventSystem::getName() const {
        return systemName;
    }
    
    void EventSystem::clearSubscribers() {
        std::lock_guard<std::mutex> lock(mutex);
        subscribers.clear();
    }
}