#pragma once

#include <RmlUi/Core.h>
#include <memory>

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

    [[nodiscard]] Rml::Context* GetContext() const { return context_; }

private:
    Rml::Context* context_ = nullptr;
};

} // namespace paradox::view
