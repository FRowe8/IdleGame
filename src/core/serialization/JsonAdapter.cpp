#include "core/serialization/JsonAdapter.h"
#include <fstream>
#include <stdexcept>

namespace paradox::core {

nlohmann::json JsonAdapter::LoadFromFile(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + file_path);
    }

    nlohmann::json json;
    try {
        file >> json;
    } catch (const nlohmann::json::exception& e) {
        throw std::runtime_error("Invalid JSON in file " + file_path + ": " + e.what());
    }

    return json;
}

void JsonAdapter::SaveToFile(const std::string& file_path, const nlohmann::json& json) {
    std::ofstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to create file: " + file_path);
    }

    // Pretty-print with 2-space indentation
    file << json.dump(2);
}

} // namespace paradox::core
