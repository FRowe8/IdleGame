#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <fstream>

namespace paradox::core {

/**
 * @brief Utility class for JSON file I/O
 */
class JsonAdapter {
public:
    /**
     * @brief Load JSON from file
     * @param file_path Path to .json file
     * @return Parsed JSON object
     * @throws std::runtime_error if file not found or invalid JSON
     */
    static nlohmann::json LoadFromFile(const std::string& file_path);

    /**
     * @brief Save JSON to file (pretty-printed)
     * @param file_path Destination path
     * @param json Data to save
     */
    static void SaveToFile(const std::string& file_path, const nlohmann::json& json);
};

} // namespace paradox::core
