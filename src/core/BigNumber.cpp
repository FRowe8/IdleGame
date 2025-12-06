#include "core/BigNumber.h"
#include <cmath>
#include <stdexcept>

namespace paradox::core {

// ============================================================================
// CONSTRUCTORS
// ============================================================================
BigNumber::BigNumber() : value_(0) {}

BigNumber::BigNumber(int64_t value) : value_(value) {}

BigNumber::BigNumber(const std::string& str_value) : value_(str_value) {}

BigNumber::BigNumber(const mppp::integer<1>& value) : value_(value) {}

// ============================================================================
// ARITHMETIC OPERATORS
// ============================================================================
BigNumber BigNumber::operator+(const BigNumber& other) const {
    return BigNumber(value_ + other.value_);
}

BigNumber BigNumber::operator-(const BigNumber& other) const {
    return BigNumber(value_ - other.value_);
}

BigNumber BigNumber::operator*(const BigNumber& other) const {
    return BigNumber(value_ * other.value_);
}

BigNumber BigNumber::operator/(const BigNumber& other) const {
    if (other.IsZero()) {
        throw std::runtime_error("Division by zero");
    }
    return BigNumber(value_ / other.value_);
}

BigNumber BigNumber::operator%(const BigNumber& other) const {
    return BigNumber(value_ % other.value_);
}

BigNumber& BigNumber::operator+=(const BigNumber& other) {
    value_ += other.value_;
    return *this;
}

BigNumber& BigNumber::operator-=(const BigNumber& other) {
    value_ -= other.value_;
    return *this;
}

BigNumber& BigNumber::operator*=(const BigNumber& other) {
    value_ *= other.value_;
    return *this;
}

BigNumber& BigNumber::operator/=(const BigNumber& other) {
    if (other.IsZero()) {
        throw std::runtime_error("Division by zero");
    }
    value_ /= other.value_;
    return *this;
}

// ============================================================================
// CONVERSIONS
// ============================================================================
std::string BigNumber::ToString() const {
    return value_.to_string();
}

std::string BigNumber::ToScientific() const {
    // TODO: Implement scientific notation (1.23e45)
    // For now, just return ToString()
    return ToString();
}

std::string BigNumber::ToHumanReadable() const {
    // TODO: Implement suffix formatting (1.23 Trillion, etc.)
    // For now, return scientific
    return ToScientific();
}

double BigNumber::ToDouble() const {
    // Lossy conversion - only use for display!
    return static_cast<double>(value_);
}

int64_t BigNumber::ToInt64() const {
    if (!value_.fits<int64_t>()) {
        throw std::overflow_error("BigNumber too large for int64_t");
    }
    return static_cast<int64_t>(value_);
}

// ============================================================================
// UTILITY
// ============================================================================
bool BigNumber::IsZero() const {
    return value_ == 0;
}

bool BigNumber::IsNegative() const {
    return value_ < 0;
}

BigNumber BigNumber::Abs() const {
    return BigNumber(mppp::abs(value_));
}

BigNumber BigNumber::Pow(int exponent) const {
    mppp::integer<1> result = 1;
    mppp::pow_ui(result, value_, static_cast<unsigned long>(exponent));
    return BigNumber(result);
}

const char* BigNumber::GetSuffixForExponent(int exponent) {
    // Idle game suffix table (short scale)
    static const char* suffixes[] = {
        "",           // 10^0
        "K",          // 10^3  - Thousand
        "M",          // 10^6  - Million
        "B",          // 10^9  - Billion
        "T",          // 10^12 - Trillion
        "Qa",         // 10^15 - Quadrillion
        "Qi",         // 10^18 - Quintillion
        "Sx",         // 10^21 - Sextillion
        "Sp",         // 10^24 - Septillion
        "Oc",         // 10^27 - Octillion
        "No",         // 10^30 - Nonillion
        "Dc",         // 10^33 - Decillion
    };

    int index = exponent / 3;
    if (index < sizeof(suffixes) / sizeof(suffixes[0])) {
        return suffixes[index];
    }
    return "???";  // Beyond supported range
}

// ============================================================================
// MATH HELPERS
// ============================================================================
BigNumber Max(const BigNumber& a, const BigNumber& b) {
    return (a > b) ? a : b;
}

BigNumber Min(const BigNumber& a, const BigNumber& b) {
    return (a < b) ? a : b;
}

} // namespace paradox::core
