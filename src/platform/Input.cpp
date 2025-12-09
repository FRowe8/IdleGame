#include "platform/Input.h"
#ifndef __EMSCRIPTEN__
#include "RmlUi_Platform_SDL.h"
#endif

namespace paradox::platform {

bool Input::PollEvents(bool& should_quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // Check for quit event first
        if (event.type == SDL_QUIT) {
            should_quit = true;
            return false;
        }

        ProcessEvent(event);
    }
    return true;
}

void Input::ProcessEvent(const SDL_Event& event) {
#ifndef __EMSCRIPTEN__
    // Forward event to RmlUi (desktop only)
    if (rmlui_context_) {
        RmlSDL::ProcessEvent(rmlui_context_, const_cast<SDL_Event&>(event));
    }
#else
    // For Emscripten, RmlUi uses built-in SDL event handling
    // Events are automatically forwarded through Emscripten's SDL implementation
    (void)event;  // Suppress unused warning
#endif

    // TODO: Handle game-specific input that RmlUi doesn't consume
}

} // namespace paradox::platform
