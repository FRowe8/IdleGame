#pragma once

#ifdef __EMSCRIPTEN__

#include <functional>

namespace paradox::platform {

/**
 * @brief Wrapper for emscripten_set_main_loop
 *
 * WebAssembly requires a different main loop structure.
 * Instead of while(running) { ... }, we use callbacks.
 */
class EmscriptenLoop {
public:
    using LoopCallback = std::function<void()>;

    static void SetMainLoop(LoopCallback callback, int fps = 0);
    static void CancelMainLoop();
};

} // namespace paradox::platform

#endif // __EMSCRIPTEN__
