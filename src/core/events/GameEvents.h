#pragma once

#include "core/events/Event.h"
#include "core/BigNumber.h"
#include <string>

namespace paradox::core {

/**
 * @brief Emitted when a resource amount changes
 */
class ResourceChangedEvent : public Event {
public:
    ResourceChangedEvent(const std::string& resource_id, const BigNumber& new_amount)
        : Event(Type::ResourceChanged)
        , resource_id_(resource_id)
        , new_amount_(new_amount)
    {}

    [[nodiscard]] std::string GetName() const override { return "ResourceChanged"; }
    [[nodiscard]] const std::string& GetResourceId() const { return resource_id_; }
    [[nodiscard]] const BigNumber& GetNewAmount() const { return new_amount_; }

private:
    std::string resource_id_;
    BigNumber new_amount_;
};

/**
 * @brief Emitted when player purchases an upgrade
 */
class UpgradePurchasedEvent : public Event {
public:
    explicit UpgradePurchasedEvent(const std::string& upgrade_id)
        : Event(Type::UpgradePurchased)
        , upgrade_id_(upgrade_id)
    {}

    [[nodiscard]] std::string GetName() const override { return "UpgradePurchased"; }
    [[nodiscard]] const std::string& GetUpgradeId() const { return upgrade_id_; }

private:
    std::string upgrade_id_;
};

/**
 * @brief Emitted when player creates a time loop (sends resources back)
 */
class TimeLoopCreatedEvent : public Event {
public:
    TimeLoopCreatedEvent(uint64_t target_tick, const BigNumber& amount)
        : Event(Type::TimeLoopCreated)
        , target_tick_(target_tick)
        , amount_(amount)
    {}

    [[nodiscard]] std::string GetName() const override { return "TimeLoopCreated"; }
    [[nodiscard]] uint64_t GetTargetTick() const { return target_tick_; }
    [[nodiscard]] const BigNumber& GetAmount() const { return amount_; }

private:
    uint64_t target_tick_;
    BigNumber amount_;
};

/**
 * @brief Emitted when paradox instability increases
 */
class ParadoxIncreasedEvent : public Event {
public:
    explicit ParadoxIncreasedEvent(float new_instability)
        : Event(Type::ParadoxIncreased)
        , new_instability_(new_instability)
    {}

    [[nodiscard]] std::string GetName() const override { return "ParadoxIncreased"; }
    [[nodiscard]] float GetNewInstability() const { return new_instability_; }

private:
    float new_instability_;
};

} // namespace paradox::core
