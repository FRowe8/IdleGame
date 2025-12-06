#pragma once

#include "core/serialization/ISerializable.h"
#include "core/BigNumber.h"
#include <string>
#include <unordered_map>

namespace paradox::core {

/**
 * @brief Central container for all game state
 *
 * This is the "single source of truth" for the entire game.
 * All game logic operates on this state.
 */
class GameState : public ISerializable {
public:
    GameState() = default;

    // ========================================================================
    // RESOURCES
    // ========================================================================
    [[nodiscard]] BigNumber GetResource(const std::string& resource_id) const;
    void AddResource(const std::string& resource_id, const BigNumber& amount);
    void SetResource(const std::string& resource_id, const BigNumber& amount);

    // ========================================================================
    // SERIALIZATION
    // ========================================================================
    [[nodiscard]] nlohmann::json ToJson() const override;
    void FromJson(const nlohmann::json& json) override;

private:
    std::unordered_map<std::string, BigNumber> resources_;
    uint64_t total_ticks_played_ = 0;
    float paradox_instability_ = 0.0f;
};

} // namespace paradox::core
