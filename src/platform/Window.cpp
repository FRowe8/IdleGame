#include "platform/Window.h"
#include <stdexcept>

namespace paradox::platform {

bool Window::Initialize(const Config& config) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        return false;
    }

    Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    if (config.fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    window_ = SDL_CreateWindow(
        config.title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        config.width,
        config.height,
        flags
    );

    if (!window_) {
        SDL_Quit();
        return false;
    }

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer_) {
        SDL_DestroyWindow(window_);
        SDL_Quit();
        return false;
    }

    return true;
}

void Window::Shutdown() {
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    SDL_Quit();
}

void Window::Clear() {
    SDL_SetRenderDrawColor(renderer_, 30, 30, 40, 255);  // Dark blue-gray
    SDL_RenderClear(renderer_);
}

void Window::Present() {
    SDL_RenderPresent(renderer_);
}

} // namespace paradox::platform
