#pragma once

#include <nlohmann/json.hpp>

namespace paradox::core {

/**
 * @brief Interface for objects that can be saved/loaded
 *
 * All persistent game state objects (GameState, TimeBank, etc.)
 * should implement this interface.
 */
class ISerializable {
public:
    virtual ~ISerializable() = default;

    /**
     * @brief Serialize object to JSON
     */
    [[nodiscard]] virtual nlohmann::json ToJson() const = 0;

    /**
     * @brief Deserialize object from JSON
     * @param json Source data
     */
    virtual void FromJson(const nlohmann::json& json) = 0;
};

} // namespace paradox::core
