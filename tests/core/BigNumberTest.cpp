#include "core/BigNumber.h"
#include <gtest/gtest.h>

using namespace paradox::core;

// ============================================================================
// CONSTRUCTION TESTS
// ============================================================================
TEST(BigNumberTest, DefaultConstructor) {
    BigNumber num;
    EXPECT_EQ(num.ToString(), "0");
    EXPECT_TRUE(num.IsZero());
}

TEST(BigNumberTest, Int64Constructor) {
    BigNumber num(12345);
    EXPECT_EQ(num.ToString(), "12345");
    EXPECT_FALSE(num.IsZero());
}

TEST(BigNumberTest, StringConstructor) {
    BigNumber num("999999999999999999999999");
    EXPECT_EQ(num.ToString(), "999999999999999999999999");
}

TEST(BigNumberTest, NegativeNumber) {
    BigNumber num(-500);
    EXPECT_TRUE(num.IsNegative());
    EXPECT_EQ(num.ToString(), "-500");
}

// ============================================================================
// ARITHMETIC TESTS
// ============================================================================
TEST(BigNumberTest, Addition) {
    BigNumber a(100);
    BigNumber b(50);
    BigNumber result = a + b;
    EXPECT_EQ(result.ToString(), "150");
}

TEST(BigNumberTest, Subtraction) {
    BigNumber a(100);
    BigNumber b(30);
    BigNumber result = a - b;
    EXPECT_EQ(result.ToString(), "70");
}

TEST(BigNumberTest, Multiplication) {
    BigNumber a(25);
    BigNumber b(4);
    BigNumber result = a * b;
    EXPECT_EQ(result.ToString(), "100");
}

TEST(BigNumberTest, Division) {
    BigNumber a(100);
    BigNumber b(4);
    BigNumber result = a / b;
    EXPECT_EQ(result.ToString(), "25");
}

TEST(BigNumberTest, DivisionByZero) {
    BigNumber a(100);
    BigNumber b(0);
    EXPECT_THROW(a / b, std::runtime_error);
}

TEST(BigNumberTest, LargeNumberArithmetic) {
    BigNumber a("1000000000000000000000");  // 10^21
    BigNumber b("2000000000000000000000");  // 2 * 10^21
    BigNumber result = a + b;
    EXPECT_EQ(result.ToString(), "3000000000000000000000");
}

// ============================================================================
// FORMATTING TESTS - ToHumanReadable()
// ============================================================================
TEST(BigNumberTest, FormatSmallNumbers) {
    EXPECT_EQ(BigNumber(0).ToHumanReadable(), "0");
    EXPECT_EQ(BigNumber(1).ToHumanReadable(), "1");
    EXPECT_EQ(BigNumber(10).ToHumanReadable(), "10");
    EXPECT_EQ(BigNumber(100).ToHumanReadable(), "100");
    EXPECT_EQ(BigNumber(999).ToHumanReadable(), "999");
}

TEST(BigNumberTest, FormatThousands) {
    EXPECT_EQ(BigNumber(1000).ToHumanReadable(), "1K");
    EXPECT_EQ(BigNumber(1500).ToHumanReadable(), "1.5K");
    EXPECT_EQ(BigNumber(1234).ToHumanReadable(), "1.23K");
    EXPECT_EQ(BigNumber(9999).ToHumanReadable(), "9.99K");
    EXPECT_EQ(BigNumber(10000).ToHumanReadable(), "10K");
    EXPECT_EQ(BigNumber(50000).ToHumanReadable(), "50K");
    EXPECT_EQ(BigNumber(999999).ToHumanReadable(), "999K");
}

TEST(BigNumberTest, FormatMillions) {
    EXPECT_EQ(BigNumber(1000000).ToHumanReadable(), "1M");
    EXPECT_EQ(BigNumber(1500000).ToHumanReadable(), "1.5M");
    EXPECT_EQ(BigNumber(2340000).ToHumanReadable(), "2.34M");
    EXPECT_EQ(BigNumber(10000000).ToHumanReadable(), "10M");
    EXPECT_EQ(BigNumber(999999999).ToHumanReadable(), "999M");
}

TEST(BigNumberTest, FormatBillions) {
    EXPECT_EQ(BigNumber(1000000000).ToHumanReadable(), "1B");
    EXPECT_EQ(BigNumber("1500000000").ToHumanReadable(), "1.5B");
    EXPECT_EQ(BigNumber("12340000000").ToHumanReadable(), "12.3B");
    EXPECT_EQ(BigNumber("123400000000").ToHumanReadable(), "123B");
}

TEST(BigNumberTest, FormatTrillions) {
    EXPECT_EQ(BigNumber("1000000000000").ToHumanReadable(), "1T");
    EXPECT_EQ(BigNumber("1500000000000").ToHumanReadable(), "1.5T");
    EXPECT_EQ(BigNumber("45600000000000").ToHumanReadable(), "45.6T");
}

TEST(BigNumberTest, FormatQuadrillions) {
    EXPECT_EQ(BigNumber("1000000000000000").ToHumanReadable(), "1Qa");
    EXPECT_EQ(BigNumber("2500000000000000").ToHumanReadable(), "2.5Qa");
}

TEST(BigNumberTest, FormatQuintillions) {
    EXPECT_EQ(BigNumber("1000000000000000000").ToHumanReadable(), "1Qi");
    EXPECT_EQ(BigNumber("7890000000000000000").ToHumanReadable(), "7.89Qi");
}

TEST(BigNumberTest, FormatSextillions) {
    EXPECT_EQ(BigNumber("1000000000000000000000").ToHumanReadable(), "1Sx");
}

TEST(BigNumberTest, FormatDecillions) {
    // 10^33
    EXPECT_EQ(BigNumber("1000000000000000000000000000000000").ToHumanReadable(), "1Dc");
}

TEST(BigNumberTest, FormatNegativeNumbers) {
    EXPECT_EQ(BigNumber(-1000).ToHumanReadable(), "-1K");
    EXPECT_EQ(BigNumber(-1500000).ToHumanReadable(), "-1.5M");
    EXPECT_EQ(BigNumber("-3400000000").ToHumanReadable(), "-3.4B");
}

TEST(BigNumberTest, FormatTrailingZeros) {
    // Should remove trailing zeros
    EXPECT_EQ(BigNumber(1000).ToHumanReadable(), "1K");  // Not "1.00K"
    EXPECT_EQ(BigNumber(10000000).ToHumanReadable(), "10M");  // Not "10.0M"
}

// ============================================================================
// FORMATTING TESTS - ToScientific()
// ============================================================================
TEST(BigNumberTest, ScientificSmallNumbers) {
    EXPECT_EQ(BigNumber(0).ToScientific(), "0");
    EXPECT_EQ(BigNumber(100).ToScientific(), "100");
    EXPECT_EQ(BigNumber(999).ToScientific(), "999");
}

TEST(BigNumberTest, ScientificLargeNumbers) {
    EXPECT_EQ(BigNumber(1000).ToScientific(), "1e3");
    EXPECT_EQ(BigNumber(1234).ToScientific(), "1.234e3");
    EXPECT_EQ(BigNumber(1000000).ToScientific(), "1e6");
    EXPECT_EQ(BigNumber("1234567890").ToScientific(), "1.234e9");
}

TEST(BigNumberTest, ScientificHugeNumbers) {
    // Test beyond normal suffix range
    std::string huge = "1";
    for (int i = 0; i < 50; ++i) huge += "0";  // 10^50
    BigNumber num(huge);
    std::string result = num.ToScientific();
    EXPECT_TRUE(result.find("e49") != std::string::npos);
}

// ============================================================================
// COMPARISON TESTS
// ============================================================================
TEST(BigNumberTest, Equality) {
    BigNumber a(100);
    BigNumber b(100);
    BigNumber c(200);
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
}

TEST(BigNumberTest, LessThan) {
    BigNumber a(50);
    BigNumber b(100);
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(b < a);
}

TEST(BigNumberTest, GreaterThan) {
    BigNumber a(100);
    BigNumber b(50);
    EXPECT_TRUE(a > b);
    EXPECT_FALSE(b > a);
}

// ============================================================================
// UTILITY TESTS
// ============================================================================
TEST(BigNumberTest, Abs) {
    BigNumber neg(-500);
    BigNumber pos = neg.Abs();
    EXPECT_EQ(pos.ToString(), "500");
}

TEST(BigNumberTest, MaxMin) {
    BigNumber a(100);
    BigNumber b(200);
    EXPECT_EQ(Max(a, b).ToString(), "200");
    EXPECT_EQ(Min(a, b).ToString(), "100");
}

TEST(BigNumberTest, Power) {
    BigNumber base(10);
    BigNumber result = base.Pow(6);
    EXPECT_EQ(result.ToString(), "1000000");
}

// ============================================================================
// EDGE CASES
// ============================================================================
TEST(BigNumberTest, VeryLargeNumbers) {
    // Test that formatting doesn't crash on huge numbers
    std::string huge = "1";
    for (int i = 0; i < 100; ++i) huge += "0";  // 10^100 (googol)
    BigNumber num(huge);

    // Should fall back to scientific for huge numbers
    std::string formatted = num.ToHumanReadable();
    EXPECT_FALSE(formatted.empty());
}

TEST(BigNumberTest, Modulo) {
    BigNumber a(17);
    BigNumber b(5);
    BigNumber result = a % b;
    EXPECT_EQ(result.ToString(), "2");
}

TEST(BigNumberTest, CompoundOperations) {
    BigNumber a(100);
    a += BigNumber(50);
    EXPECT_EQ(a.ToString(), "150");

    a *= BigNumber(2);
    EXPECT_EQ(a.ToString(), "300");

    a -= BigNumber(50);
    EXPECT_EQ(a.ToString(), "250");

    a /= BigNumber(5);
    EXPECT_EQ(a.ToString(), "50");
}
