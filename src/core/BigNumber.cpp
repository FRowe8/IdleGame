#include "core/BigNumber.h"
#include <cmath>
#include <limits>
#include <stdexcept>
#include <sstream>
#include <iomanip>

namespace paradox::core {

// ============================================================================
// CONSTRUCTORS
// ============================================================================
#ifdef PARADOX_USE_MPPP
BigNumber::BigNumber() : value_(0) {}
BigNumber::BigNumber(int64_t value) : value_(value) {}
BigNumber::BigNumber(const std::string& str_value) : value_(str_value) {}
BigNumber::BigNumber(const mppp::integer<1>& value) : value_(value) {}
#else
// Simplified implementation for WebAssembly
BigNumber::BigNumber() : value_(0.0) {}
BigNumber::BigNumber(int64_t value) : value_(static_cast<double>(value)) {}
BigNumber::BigNumber(const std::string& str_value) {
    try {
        value_ = std::stod(str_value);
    } catch (...) {
        value_ = 0.0;
    }
}
#endif

// ============================================================================
// ARITHMETIC OPERATORS
// ============================================================================
BigNumber BigNumber::operator+(const BigNumber& other) const {
#ifdef PARADOX_USE_MPPP
    return BigNumber(value_ + other.value_);
#else
    BigNumber result;
    result.value_ = value_ + other.value_;
    return result;
#endif
}

BigNumber BigNumber::operator-(const BigNumber& other) const {
#ifdef PARADOX_USE_MPPP
    return BigNumber(value_ - other.value_);
#else
    BigNumber result;
    result.value_ = value_ - other.value_;
    return result;
#endif
}

BigNumber BigNumber::operator*(const BigNumber& other) const {
#ifdef PARADOX_USE_MPPP
    return BigNumber(value_ * other.value_);
#else
    BigNumber result;
    result.value_ = value_ * other.value_;
    return result;
#endif
}

BigNumber BigNumber::operator/(const BigNumber& other) const {
    if (other.IsZero()) {
        throw std::runtime_error("Division by zero");
    }
#ifdef PARADOX_USE_MPPP
    return BigNumber(value_ / other.value_);
#else
    BigNumber result;
    result.value_ = value_ / other.value_;
    return result;
#endif
}

BigNumber BigNumber::operator%(const BigNumber& other) const {
#ifdef PARADOX_USE_MPPP
    return BigNumber(value_ % other.value_);
#else
    // For double, use fmod
    BigNumber result;
    result.value_ = std::fmod(value_, other.value_);
    return result;
#endif
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
// COMPARISON OPERATORS
// ============================================================================
bool BigNumber::operator==(const BigNumber& other) const {
    return value_ == other.value_;
}

bool BigNumber::operator!=(const BigNumber& other) const {
    return value_ != other.value_;
}

bool BigNumber::operator<(const BigNumber& other) const {
    return value_ < other.value_;
}

bool BigNumber::operator<=(const BigNumber& other) const {
    return value_ <= other.value_;
}

bool BigNumber::operator>(const BigNumber& other) const {
    return value_ > other.value_;
}

bool BigNumber::operator>=(const BigNumber& other) const {
    return value_ >= other.value_;
}

// ============================================================================
// CONVERSIONS
// ============================================================================
std::string BigNumber::ToString() const {
#ifdef PARADOX_USE_MPPP
    return value_.to_string();
#else
    // For double, convert to string with full precision
    std::ostringstream oss;
    oss << std::setprecision(15) << value_;
    return oss.str();
#endif
}

std::string BigNumber::ToScientific() const {
    if (IsZero()) {
        return "0";
    }

#ifdef PARADOX_USE_MPPP
    // Get absolute value for calculation
    BigNumber abs_val = Abs();
    std::string str = abs_val.ToString();

    // Calculate exponent (number of digits - 1)
    int exponent = static_cast<int>(str.length()) - 1;

    // Edge case: small numbers (< 1000)
    if (exponent < 3) {
        return ToString();
    }

    // Get mantissa: divide by 10^exponent to get value between 1 and 10
    // For scientific notation: 1234567 -> 1.234567e6
    std::string mantissa = str.substr(0, 1);
    if (str.length() > 1) {
        mantissa += ".";
        // Show up to 3 decimal places
        mantissa += str.substr(1, std::min(3, static_cast<int>(str.length()) - 1));
    }

    // Add sign if negative
    std::string result = IsNegative() ? "-" : "";
    result += mantissa + "e" + std::to_string(exponent);

    return result;
#else
    // For double, use scientific notation directly
    std::ostringstream oss;
    oss << std::scientific << std::setprecision(3) << value_;
    return oss.str();
#endif
}

std::string BigNumber::ToHumanReadable() const {
    if (IsZero()) {
        return "0";
    }

#ifdef PARADOX_USE_MPPP
    // Get absolute value for calculation
    BigNumber abs_val = Abs();
    std::string str = abs_val.ToString();
    int num_digits = static_cast<int>(str.length());

    // Numbers less than 1,000: show as-is
    if (num_digits < 4) {
        return ToString();
    }

    // Determine suffix group (K, M, B, T, etc.)
    // Exponent group: 3 digits = K (10^3), 6 digits = M (10^6), etc.
    int exponent_group = (num_digits - 1) / 3;
    const char* suffix = GetSuffixForExponent(exponent_group * 3);

    // Calculate how many digits to show before decimal point
    int leading_digits = num_digits - (exponent_group * 3);

    // Build mantissa: show 3 significant figures total
    std::string mantissa = str.substr(0, leading_digits);

    // Add decimal places (up to 3 sig figs total)
    int decimals_to_show = std::min(3 - leading_digits, 2);
    if (decimals_to_show > 0 && leading_digits < static_cast<int>(str.length())) {
        mantissa += ".";
        mantissa += str.substr(leading_digits, decimals_to_show);
    }

    // Remove trailing zeros after decimal point
    size_t decimal_pos = mantissa.find('.');
    if (decimal_pos != std::string::npos) {
        size_t last_nonzero = mantissa.find_last_not_of('0');
        if (last_nonzero >= decimal_pos) {
            mantissa = mantissa.substr(0, last_nonzero + 1);
            // Remove decimal point if no decimals remain
            if (mantissa.back() == '.') {
                mantissa.pop_back();
            }
        }
    }

    // Add sign if negative
    std::string result = IsNegative() ? "-" : "";
    result += mantissa;

    // Add suffix (unless it's "???")
    if (std::string(suffix) != "???") {
        result += suffix;
    } else {
        // Fall back to scientific for huge numbers
        return ToScientific();
    }

    return result;
#else
    // Simplified for double
    double abs_val = std::abs(value_);

    if (abs_val < 1000) {
        return ToString();
    }

    // Determine suffix
    int exponent = static_cast<int>(std::log10(abs_val));
    int exponent_group = exponent / 3;
    const char* suffix = GetSuffixForExponent(exponent_group * 3);

    // Calculate mantissa
    double divisor = std::pow(10.0, exponent_group * 3);
    double mantissa = abs_val / divisor;

    // Format
    std::ostringstream oss;
    if (IsNegative()) oss << "-";
    oss << std::setprecision(3) << mantissa;

    std::string result = oss.str();
    // Remove trailing zeros
    size_t decimal_pos = result.find('.');
    if (decimal_pos != std::string::npos) {
        result.erase(result.find_last_not_of('0') + 1);
        if (result.back() == '.') result.pop_back();
    }

    result += suffix;
    return result;
#endif
}

double BigNumber::ToDouble() const {
    // Lossy conversion - only use for display!
#ifdef PARADOX_USE_MPPP
    return static_cast<double>(value_);
#else
    return value_;
#endif
}

int64_t BigNumber::ToInt64() const {
#ifdef PARADOX_USE_MPPP
    // Check if value fits in int64_t range
    mppp::integer<1> int64_max(std::numeric_limits<int64_t>::max());
    mppp::integer<1> int64_min(std::numeric_limits<int64_t>::min());

    if (value_ > int64_max || value_ < int64_min) {
        throw std::overflow_error("BigNumber too large for int64_t");
    }

    return static_cast<int64_t>(value_);
#else
    if (value_ > static_cast<double>(std::numeric_limits<int64_t>::max()) ||
        value_ < static_cast<double>(std::numeric_limits<int64_t>::min())) {
        throw std::overflow_error("BigNumber too large for int64_t");
    }
    return static_cast<int64_t>(value_);
#endif
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
#ifdef PARADOX_USE_MPPP
    return BigNumber(mppp::abs(value_));
#else
    BigNumber result;
    result.value_ = std::abs(value_);
    return result;
#endif
}

BigNumber BigNumber::Pow(int exponent) const {
#ifdef PARADOX_USE_MPPP
    mppp::integer<1> result = 1;
    mppp::pow_ui(result, value_, static_cast<unsigned long>(exponent));
    return BigNumber(result);
#else
    BigNumber result;
    result.value_ = std::pow(value_, static_cast<double>(exponent));
    return result;
#endif
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
    if (index < static_cast<int>(sizeof(suffixes) / sizeof(suffixes[0]))) {
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
