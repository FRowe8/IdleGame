#pragma once

#include <SDL.h>
#include <RmlUi/Core/Context.h>

namespace paradox::platform {

/**
 * @brief SDL2 input handling with RmlUi integration
 */
class Input {
public:
    // Set the RmlUi context to forward events to
    void SetRmlUiContext(Rml::Context* context) { rmlui_context_ = context; }

    bool PollEvents(bool& should_quit);
    void ProcessEvent(const SDL_Event& event);

private:
    Rml::Context* rmlui_context_ = nullptr;
};

} // namespace paradox::platform
