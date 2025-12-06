#include "platform/Window.h"
#include <stdexcept>

namespace paradox::platform {

bool Window::Initialize(const Config& config) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        return false;
    }

    // Set OpenGL attributes for RmlUi
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
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

    // Create OpenGL context instead of SDL renderer
    gl_context_ = SDL_GL_CreateContext(window_);
    if (!gl_context_) {
        SDL_DestroyWindow(window_);
        SDL_Quit();
        return false;
    }

    // Enable vsync
    SDL_GL_SetSwapInterval(1);

    return true;
}

void Window::Shutdown() {
    if (gl_context_) {
        SDL_GL_DeleteContext(gl_context_);
        gl_context_ = nullptr;
    }
    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    SDL_Quit();
}

void Window::Clear() {
    glClearColor(0.12f, 0.12f, 0.16f, 1.0f);  // Dark blue-gray
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::Present() {
    SDL_GL_SwapWindow(window_);
}

void Window::GetSize(int& width, int& height) const {
    SDL_GetWindowSize(window_, &width, &height);
}

} // namespace paradox::platform
