#pragma once

#include <SDL.h>

namespace paradox::platform {

/**
 * @brief SDL2 input handling
 */
class Input {
public:
    bool PollEvents(bool& should_quit);
    void ProcessEvent(const SDL_Event& event);

private:
    // TODO: Input state tracking
};

} // namespace paradox::platform
