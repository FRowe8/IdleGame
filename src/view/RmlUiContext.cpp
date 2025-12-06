#include "view/RmlUiContext.h"
#include <RmlUi/Core.h>

namespace paradox::view {

bool RmlUiContext::Initialize() {
    Rml::SetRenderInterface(nullptr);  // TODO: Implement SDL2 render interface
    Rml::SetSystemInterface(nullptr);  // TODO: Implement SDL2 system interface

    Rml::Initialise();

    context_ = Rml::CreateContext("main", Rml::Vector2i(1280, 720));
    return context_ != nullptr;
}

void RmlUiContext::Shutdown() {
    if (context_) {
        context_->RemoveReference();
        context_ = nullptr;
    }
    Rml::Shutdown();
}

void RmlUiContext::Update() {
    if (context_) {
        context_->Update();
    }
}

void RmlUiContext::Render() {
    if (context_) {
        context_->Render();
    }
}

} // namespace paradox::view
