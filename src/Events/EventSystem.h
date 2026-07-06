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

namespace SGE::EVENTS {
    class EventSystem : public IEventSystem {
    private:
        std::unordered_map<std::string, std::vector<std::function<void(const IEvent&)>>> subscribers;
        std::string systemName;
        mutable std::mutex mutex;
        
    public:
        EventSystem(const std::string& name = "DefaultEventSystem");
        ~EventSystem() override = default;
        
        // IEventSystem interface
        void subscribe(const std::string& eventType, 
                      std::function<void(const IEvent&)> handler) override;
        void unsubscribe(const std::string& eventType) override;
        void publish(const IEvent& event) override;
        std::string getName() const override;
        
        // Event system specific methods
        void clearSubscribers();
    };
};

#endif // GLCPP_EVENTSYSTEM_H