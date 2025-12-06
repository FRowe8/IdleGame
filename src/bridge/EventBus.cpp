#include "bridge/EventBus.h"
#include <algorithm>

namespace paradox::bridge {

void EventBus::Unsubscribe(uint32_t subscription_id) {
    for (auto& [type, listeners] : listeners_) {
        auto it = std::remove_if(listeners.begin(), listeners.end(),
            [subscription_id](const Listener& l) { return l.id == subscription_id; });
        listeners.erase(it, listeners.end());
    }
}

void EventBus::Publish(const core::Event& event) {
    auto it = listeners_.find(event.GetType());
    if (it == listeners_.end()) {
        return;  // No subscribers for this event type
    }

    for (auto& listener : it->second) {
        listener.callback(event);
    }
}

} // namespace paradox::bridge
