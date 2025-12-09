#pragma once

#include <RmlUi/Core.h>
#include <string>
#include <memory>

#ifdef __EMSCRIPTEN__
#include "platform/emscripten/RmlUiSystemInterface.h"
#endif

namespace paradox::view {

/**
 * @brief Wrapper for RmlUi initialization and context management
 */
class RmlUiContext {
public:
    bool Initialize();
    void Shutdown();

    void Update();
    void Render();

    // Load an RmlUi document from file
    Rml::ElementDocument* LoadDocument(const std::string& rml_path);

    // Set viewport dimensions
    void SetDimensions(int width, int height);

    [[nodiscard]] Rml::Context* GetContext() const { return context_; }

private:
    Rml::Context* context_ = nullptr;

#ifdef __EMSCRIPTEN__
    std::unique_ptr<platform::RmlUiSystemInterface> system_interface_;
#endif
};

} // namespace paradox::view
