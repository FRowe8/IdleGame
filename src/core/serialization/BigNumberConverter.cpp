#include "core/serialization/BigNumberConverter.h"

namespace paradox::core {

void to_json(nlohmann::json& j, const BigNumber& number) {
    // Serialize as string to preserve precision
    j = number.ToString();
}

void from_json(const nlohmann::json& j, BigNumber& number) {
    if (j.is_string()) {
        number = BigNumber(j.get<std::string>());
    } else if (j.is_number_integer()) {
        number = BigNumber(j.get<int64_t>());
    } else {
        throw std::runtime_error("Cannot deserialize BigNumber from JSON type: " + j.type_name());
    }
}

} // namespace paradox::core
