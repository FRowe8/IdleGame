#pragma once

namespace paradox::core {

/**
 * @brief Manages causal loop validation and paradox tracking
 */
class ParadoxEngine {
public:
    ParadoxEngine() = default;

    float GetInstability() const { return instability_; }
    void AddInstability(float amount) { instability_ += amount; }

private:
    float instability_ = 0.0f;
};

} // namespace paradox::core
