#pragma once

#include "core/BigNumber.h"

namespace paradox::core {

/**
 * @brief Handles timeline collapse (prestige/reset mechanics)
 */
class PrestigeSystem {
public:
    PrestigeSystem() = default;

    BigNumber CalculateTemporalAnchors() const;
    void ResetTimeline();

private:
    BigNumber total_credits_earned_{0};
};

} // namespace paradox::core
