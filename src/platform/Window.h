#pragma once

#include <SDL.h>
#include <string>

namespace paradox::platform {

/**
 * @brief SDL2 window wrapper
 */
class Window {
public:
    struct Config {
        std::string title = "Paradox Protocol";
        int width = 1280;
        int height = 720;
        bool fullscreen = false;
    };

    bool Initialize(const Config& config);
    void Shutdown();

    void Clear();
    void Present();

    [[nodiscard]] SDL_Window* GetSDLWindow() const { return window_; }
    [[nodiscard]] SDL_Renderer* GetRenderer() const { return renderer_; }
    [[nodiscard]] bool IsInitialized() const { return window_ != nullptr; }

private:
    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
};

} // namespace paradox::platform
