#pragma once

#include "core/events/Event.h"
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace paradox::bridge {

/**
 * @brief Central event dispatcher (Observer pattern)
 *
 * Core emits events → EventBus dispatches → UI listeners react
 *
 * Example:
 *   EventBus bus;
 *
 *   // Subscribe to resource changes
 *   bus.Subscribe<ResourceChangedEvent>([](const ResourceChangedEvent& e) {
 *       ui_label.SetText(e.GetNewAmount().ToHumanReadable());
 *   });
 *
 *   // Emit from Core
 *   bus.Publish(ResourceChangedEvent("credits", BigNumber(1000)));
 */
class EventBus {
public:
    using EventCallback = std::function<void(const core::Event&)>;

    /**
     * @brief Subscribe to a specific event type
     * @tparam T Event type (must inherit from core::Event)
     * @param callback Function to invoke when event is published
     * @return Subscription ID (can be used to unsubscribe later)
     */
    template<typename T>
    uint32_t Subscribe(std::function<void(const T&)> callback) {
        auto type = T(T::Type{}).GetType();  // Hack to get Type enum
        uint32_t id = next_id_++;

        listeners_[type].emplace_back(id, [callback](const core::Event& e) {
            callback(static_cast<const T&>(e));
        });

        return id;
    }

    /**
     * @brief Unsubscribe a specific listener
     */
    void Unsubscribe(uint32_t subscription_id);

    /**
     * @brief Publish an event to all subscribers
     */
    void Publish(const core::Event& event);

private:
    struct Listener {
        uint32_t id;
        EventCallback callback;

        Listener(uint32_t id, EventCallback cb)
            : id(id), callback(std::move(cb)) {}
    };

    std::unordered_map<core::Event::Type, std::vector<Listener>> listeners_;
    uint32_t next_id_ = 1;
};

} // namespace paradox::bridge
