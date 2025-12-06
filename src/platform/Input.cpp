#include "platform/Input.h"
#include "RmlUi_Platform_SDL.h"

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
    // Forward event to RmlUi
    if (rmlui_context_) {
        RmlSDL::ProcessEvent(rmlui_context_, const_cast<SDL_Event&>(event));
    }

    // TODO: Handle game-specific input that RmlUi doesn't consume
}

} // namespace paradox::platform
