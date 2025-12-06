#include "core/TimeBank.h"

namespace paradox::core {

void TimeBank::Update(float delta_time) {
    // Compound interest: balance *= (1 + rate)^dt
    // Simplified for stub: balance += balance * rate * dt
    if (!balance_.IsZero()) {
        BigNumber interest = balance_ * BigNumber(static_cast<int64_t>(interest_rate_ * delta_time * 1000)) / BigNumber(1000);
        balance_ += interest;
    }
}

void TimeBank::Deposit(const BigNumber& amount) {
    balance_ += amount;
}

} // namespace paradox::core
