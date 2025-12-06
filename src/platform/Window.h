#pragma once

#include <SDL.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <SDL_opengl.h>
#endif
#include <string>

namespace paradox::platform {

/**
 * @brief SDL2 window wrapper with OpenGL context
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
    void GetSize(int& width, int& height) const;

    [[nodiscard]] SDL_Window* GetSDLWindow() const { return window_; }
    [[nodiscard]] SDL_GLContext GetGLContext() const { return gl_context_; }
    [[nodiscard]] bool IsInitialized() const { return window_ != nullptr; }

private:
    SDL_Window* window_ = nullptr;
    SDL_GLContext gl_context_ = nullptr;
};

} // namespace paradox::platform
