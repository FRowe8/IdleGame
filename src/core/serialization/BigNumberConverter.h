#pragma once

#include "core/BigNumber.h"
#include <nlohmann/json.hpp>

namespace paradox::core {

/**
 * @brief JSON serialization adapter for BigNumber
 *
 * PRODUCTION CONSTRAINT: Serialization Strategy
 * ==============================================
 * Standard JSON number types cannot hold idle game values (e.g., 10^308).
 * We store BigNumbers as BASE-10 STRINGS in JSON.
 *
 * Example:
 *   BigNumber(123456789) → JSON: "123456789"
 *   BigNumber("99999999999999999999") → JSON: "99999999999999999999"
 *
 * This adapter hooks into nlohmann/json's conversion system:
 *   json j = my_big_number;  // Automatically uses to_json()
 *   BigNumber n = j.get<BigNumber>();  // Automatically uses from_json()
 */

// ============================================================================
// NLOHMANN JSON CONVERTERS (ADL - Argument Dependent Lookup)
// ============================================================================
void to_json(nlohmann::json& j, const BigNumber& number);
void from_json(const nlohmann::json& j, BigNumber& number);

} // namespace paradox::core

// ============================================================================
// OPTIONAL: Custom JSON serializer (if ADL doesn't work)
// ============================================================================
namespace nlohmann {
    template <>
    struct adl_serializer<paradox::core::BigNumber> {
        static void to_json(json& j, const paradox::core::BigNumber& number) {
            paradox::core::to_json(j, number);
        }

        static void from_json(const json& j, paradox::core::BigNumber& number) {
            paradox::core::from_json(j, number);
        }
    };
}
