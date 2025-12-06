#pragma once

#include <RmlUi/Core.h>
#include <string>

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
};

} // namespace paradox::view
