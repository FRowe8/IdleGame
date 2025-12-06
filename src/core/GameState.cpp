#include "core/GameState.h"
#include "core/serialization/BigNumberConverter.h"

namespace paradox::core {

BigNumber GameState::GetResource(const std::string& resource_id) const {
    auto it = resources_.find(resource_id);
    if (it == resources_.end()) {
        return BigNumber(0);
    }
    return it->second;
}

void GameState::AddResource(const std::string& resource_id, const BigNumber& amount) {
    resources_[resource_id] += amount;
}

void GameState::SetResource(const std::string& resource_id, const BigNumber& amount) {
    resources_[resource_id] = amount;
}

nlohmann::json GameState::ToJson() const {
    nlohmann::json j;
    j["total_ticks_played"] = total_ticks_played_;
    j["paradox_instability"] = paradox_instability_;

    nlohmann::json resources_json;
    for (const auto& [id, amount] : resources_) {
        resources_json[id] = amount;  // Uses BigNumberConverter automatically
    }
    j["resources"] = resources_json;

    return j;
}

void GameState::FromJson(const nlohmann::json& json) {
    total_ticks_played_ = json.value("total_ticks_played", 0ULL);
    paradox_instability_ = json.value("paradox_instability", 0.0f);

    if (json.contains("resources")) {
        for (auto& [key, val] : json["resources"].items()) {
            resources_[key] = val.get<BigNumber>();  // Uses BigNumberConverter
        }
    }
}

} // namespace paradox::core
