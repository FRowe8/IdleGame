#pragma once

#ifdef PARADOX_USE_MPPP
#include <mp++/integer.hpp>
#include <mp++/rational.hpp>
#endif

#include <string>
#include <compare>
#include <cstdint>

namespace paradox::core {

/**
 * @brief Arbitrary precision number wrapper for idle game scaling
 *
 * Idle games need to handle absurdly large numbers (e.g., 10^308+).
 *
 * Implementation:
 * - Desktop/Native: Uses mp++ (GMP backend) for true arbitrary precision
 * - WebAssembly: Uses double for compatibility (limited to ~10^308)
 *
 * Design Constraints:
 * - MUST be serializable to JSON as string (see BigNumberConverter)
 * - MUST support all arithmetic operators (+, -, *, /, %)
 * - MUST provide human-readable formatting (1.23e45 → "1.23 Quattuordecillion")
 */
class BigNumber {
public:
    // ========================================================================
    // CONSTRUCTORS
    // ========================================================================
    BigNumber();
    explicit BigNumber(int64_t value);
    explicit BigNumber(const std::string& str_value);  // For JSON deserialization

#ifdef PARADOX_USE_MPPP
    explicit BigNumber(const mppp::integer<1>& value);
#endif

    // ========================================================================
    // ARITHMETIC OPERATORS
    // ========================================================================
    BigNumber operator+(const BigNumber& other) const;
    BigNumber operator-(const BigNumber& other) const;
    BigNumber operator*(const BigNumber& other) const;
    BigNumber operator/(const BigNumber& other) const;
    BigNumber operator%(const BigNumber& other) const;

    BigNumber& operator+=(const BigNumber& other);
    BigNumber& operator-=(const BigNumber& other);
    BigNumber& operator*=(const BigNumber& other);
    BigNumber& operator/=(const BigNumber& other);

    // ========================================================================
    // COMPARISON OPERATORS
    // ========================================================================
    bool operator==(const BigNumber& other) const;
    bool operator!=(const BigNumber& other) const;
    bool operator<(const BigNumber& other) const;
    bool operator<=(const BigNumber& other) const;
    bool operator>(const BigNumber& other) const;
    bool operator>=(const BigNumber& other) const;

    // ========================================================================
    // CONVERSIONS
    // ========================================================================
    [[nodiscard]] std::string ToString() const;              // Base-10 string
    [[nodiscard]] std::string ToScientific() const;          // "1.23e45"
    [[nodiscard]] std::string ToHumanReadable() const;       // "1.23 Quattuordecillion"

    [[nodiscard]] double ToDouble() const;                   // Lossy conversion (for display only)
    [[nodiscard]] int64_t ToInt64() const;                   // Throws if out of range

    // ========================================================================
    // UTILITY
    // ========================================================================
    [[nodiscard]] bool IsZero() const;
    [[nodiscard]] bool IsNegative() const;
    [[nodiscard]] BigNumber Abs() const;
    [[nodiscard]] BigNumber Pow(int exponent) const;

    // ========================================================================
    // SERIALIZATION SUPPORT
    // ========================================================================
#ifdef PARADOX_USE_MPPP
    // Used by BigNumberConverter for JSON serialization
    [[nodiscard]] const mppp::integer<1>& GetRawValue() const { return value_; }
#endif

private:
#ifdef PARADOX_USE_MPPP
    mppp::integer<1> value_;  // Dynamic-size integer (GMP backend)
#else
    double value_;  // Simplified implementation for WebAssembly (limited precision)
#endif

    // Suffix table for human-readable formatting
    static const char* GetSuffixForExponent(int exponent);
};

// ============================================================================
// MATH HELPERS (Outside class for cleaner API)
// ============================================================================
BigNumber Max(const BigNumber& a, const BigNumber& b);
BigNumber Min(const BigNumber& a, const BigNumber& b);

} // namespace paradox::core
