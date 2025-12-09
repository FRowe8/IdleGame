#include "RmlUiSystemInterface.h"
#include <emscripten.h>
#include <iostream>

namespace paradox::platform {

double RmlUiSystemInterface::GetElapsedTime() {
    // emscripten_get_now() returns time in milliseconds
    return emscripten_get_now() / 1000.0;
}

bool RmlUiSystemInterface::LogMessage(Rml::Log::Type type, const Rml::String& message) {
    const char* type_str = "Unknown";
    switch (type) {
        case Rml::Log::Type::LT_ALWAYS:
            type_str = "Always";
            break;
        case Rml::Log::Type::LT_ERROR:
            type_str = "Error";
            break;
        case Rml::Log::Type::LT_ASSERT:
            type_str = "Assert";
            break;
        case Rml::Log::Type::LT_WARNING:
            type_str = "Warning";
            break;
        case Rml::Log::Type::LT_INFO:
            type_str = "Info";
            break;
        case Rml::Log::Type::LT_DEBUG:
            type_str = "Debug";
            break;
        case Rml::Log::Type::LT_MAX:
            type_str = "Max";
            break;
    }

    // Log to browser console (emscripten redirects stdout/stderr)
    if (type == Rml::Log::Type::LT_ERROR || type == Rml::Log::Type::LT_ASSERT) {
        std::cerr << "[RmlUi " << type_str << "] " << message.c_str() << std::endl;
    } else if (type == Rml::Log::Type::LT_WARNING) {
        std::cout << "⚠️  [RmlUi " << type_str << "] " << message.c_str() << std::endl;
    } else {
        std::cout << "[RmlUi " << type_str << "] " << message.c_str() << std::endl;
    }

    return true;
}

} // namespace paradox::platform
