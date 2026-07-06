//
// Created by Developer on [date].
//

#include "EventSystem.h"
#include <algorithm>

namespace SGE::EVENTS {
    EventSystem::EventSystem(const std::string& name) : systemName(name) {}
    
    void EventSystem::subscribe(const std::string& eventType, 
                               std::function<void(const IEvent&)> handler) {
        std::lock_guard<std::mutex> lock(mutex);
        subscribers[eventType].push_back(handler);
    }
    
    void EventSystem::unsubscribe(const std::string& eventType) {
        std::lock_guard<std::mutex> lock(mutex);
        subscribers.erase(eventType);
    }
    
    void EventSystem::publish(const IEvent& event) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = subscribers.find(event.getType());
        if (it != subscribers.end()) {
            for (const auto& handler : it->second) {
                handler(event);
            }
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