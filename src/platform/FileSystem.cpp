#include "platform/FileSystem.h"
#include <fstream>
#include <sstream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace paradox::platform {

std::string FileSystem::GetSaveDirectory() {
#ifdef __EMSCRIPTEN__
    return "/saves";  // Emscripten virtual file system
#else
    // TODO: Use SDL_GetPrefPath() for proper cross-platform save location
    return "./saves";
#endif
}

std::string FileSystem::GetDataDirectory() {
#ifdef __EMSCRIPTEN__
    return "/data";
#else
    return "./data";
#endif
}

bool FileSystem::FileExists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

std::string FileSystem::ReadFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool FileSystem::WriteFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }

    file << content;
    return true;
}

} // namespace paradox::platform
