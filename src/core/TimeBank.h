#pragma once

#include "core/BigNumber.h"

namespace paradox::core {

/**
 * @brief Handles time-based interest accrual
 * Resources in the time bank generate compound interest
 */
class TimeBank {
public:
    TimeBank() = default;

    void Update(float delta_time);
    void Deposit(const BigNumber& amount);
    BigNumber GetBalance() const { return balance_; }

private:
    BigNumber balance_{0};
    float interest_rate_ = 0.01f;  // 1% per second
};

} // namespace paradox::core
