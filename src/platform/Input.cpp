#include "platform/Input.h"

namespace paradox::platform {

bool Input::PollEvents(bool& should_quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ProcessEvent(event);

        if (event.type == SDL_QUIT) {
            should_quit = true;
            return false;
        }
    }
    return true;
}

void Input::ProcessEvent(const SDL_Event& event) {
    // TODO: Forward to RmlUi
    // TODO: Handle game-specific input
}

} // namespace paradox::platform
