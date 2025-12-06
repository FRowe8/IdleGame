#pragma once

#include <string>

namespace paradox::platform {

/**
 * @brief Platform-agnostic file I/O
 *
 * Handles platform-specific paths:
 * - iOS: App sandbox
 * - Android: Internal storage
 * - Web: IndexedDB via Emscripten
 * - PC: User documents folder
 */
class FileSystem {
public:
    static std::string GetSaveDirectory();
    static std::string GetDataDirectory();

    static bool FileExists(const std::string& path);
    static std::string ReadFile(const std::string& path);
    static bool WriteFile(const std::string& path, const std::string& content);
};

} // namespace paradox::platform
