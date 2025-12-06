#ifdef __EMSCRIPTEN__

#include "platform/emscripten/EmscriptenLoop.h"
#include <emscripten.h>

namespace paradox::platform {

static EmscriptenLoop::LoopCallback g_loop_callback;

static void EmscriptenMainLoopCallback() {
    if (g_loop_callback) {
        g_loop_callback();
    }
}

void EmscriptenLoop::SetMainLoop(LoopCallback callback, int fps) {
    g_loop_callback = std::move(callback);
    emscripten_set_main_loop(EmscriptenMainLoopCallback, fps, 1);
}

void EmscriptenLoop::CancelMainLoop() {
    emscripten_cancel_main_loop();
    g_loop_callback = nullptr;
}

} // namespace paradox::platform

#endif // __EMSCRIPTEN__
