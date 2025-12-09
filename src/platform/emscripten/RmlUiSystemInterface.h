#pragma once

#include <RmlUi/Core/SystemInterface.h>
#include <string>

namespace paradox::platform {

/**
 * @brief Emscripten-compatible RmlUi system interface
 *
 * Provides file access and timing functionality for RmlUi in WebAssembly.
 * Uses Emscripten's virtual filesystem and high-resolution timer.
 */
class RmlUiSystemInterface : public Rml::SystemInterface {
public:
    RmlUiSystemInterface() = default;
    ~RmlUiSystemInterface() override = default;

    // Get elapsed time in seconds since program start
    double GetElapsedTime() override;

    // Log messages from RmlUi
    bool LogMessage(Rml::Log::Type type, const Rml::String& message) override;
};

} // namespace paradox::platform
