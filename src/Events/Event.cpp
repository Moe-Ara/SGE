#include "Event.h"

namespace SGE::EVENTS {

Event::Event(const std::string& type, const std::string& name)
    : eventType(type), eventName(name) {}

std::string Event::getType() const {
    return eventType;
}

std::string Event::getName() const {
    return eventName;
}

} // namespace SGE::EVENTS
