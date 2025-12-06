#pragma once

#include <cstdint>
#include <string>

namespace paradox::core {

/**
 * @brief Base class for all game events
 *
 * Events flow from Core → Bridge → View
 * Used to decouple game logic from UI updates
 */
class Event {
public:
    enum class Type {
        ResourceChanged,
        UpgradePurchased,
        TimeLoopCreated,
        ParadoxIncreased,
        TimelineCollapsed,
        PrestigeCompleted
    };

    explicit Event(Type type) : type_(type) {}
    virtual ~Event() = default;

    [[nodiscard]] Type GetType() const { return type_; }
    [[nodiscard]] virtual std::string GetName() const = 0;

private:
    Type type_;
};

} // namespace paradox::core
