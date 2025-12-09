#include "view/RmlUiContext.h"
#include <RmlUi/Core.h>

namespace paradox::view {

bool RmlUiContext::Initialize() {
    // NOTE: Backends are initialized globally via RmlSDL::Initialize() and RmlGL2::Initialize()
    // This is called from main.cpp before this function

    Rml::Initialise();

    context_ = Rml::CreateContext("main", Rml::Vector2i(1280, 720));
    if (!context_) {
        return false;
    }

    // TODO: Load default fonts (currently commented out - fonts not included)
    // Rml::LoadFontFace("data/fonts/LatoLatin-Regular.ttf", true, Rml::Style::FontWeight::Normal);
    // Rml::LoadFontFace("data/fonts/LatoLatin-Bold.ttf", true, Rml::Style::FontWeight::Bold);

    return true;
}

void RmlUiContext::Shutdown() {
    if (context_) {
        context_->RemoveReference();
        context_ = nullptr;
    }
    Rml::Shutdown();

    // NOTE: Backends are shut down globally via RmlSDL::Shutdown() and RmlGL2::Shutdown()
    // This is called from main.cpp after this function
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

Rml::ElementDocument* RmlUiContext::LoadDocument(const std::string& rml_path) {
    if (!context_) {
        return nullptr;
    }

    Rml::ElementDocument* document = context_->LoadDocument(rml_path.c_str());
    if (document) {
        document->Show();
    }
    return document;
}

void RmlUiContext::SetDimensions(int width, int height) {
    if (context_) {
        context_->SetDimensions(Rml::Vector2i(width, height));
    }
}

} // namespace paradox::view
