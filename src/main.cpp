/**
 * PARADOX PROTOCOL - Main Entry Point
 * ====================================
 * Time Travel Resource Arbitrage Idle Game
 * Architecture: Core → Bridge → View → Platform
 */

#include "platform/Window.h"
#include "platform/Input.h"
#include "view/RmlUiContext.h"
#include "core/TickSystem.h"
#include "core/GameState.h"
#include "core/BigNumber.h"
#include "bridge/UpdateThrottle.h"

#include <iostream>
#include <chrono>

#ifdef __EMSCRIPTEN__
#include "platform/emscripten/EmscriptenLoop.h"
#endif

using namespace paradox;

// ============================================================================
// GLOBAL GAME OBJECTS (for simplicity in Hello World)
// ============================================================================
struct GameContext {
    platform::Window window;
    platform::Input input;
    view::RmlUiContext rmlui;

    core::GameState game_state;
    core::TickSystem tick_system{60};  // 60 Hz tick rate

    bridge::UpdateThrottle ui_throttle{0.15f};  // Update UI every 150ms

    bool should_quit = false;
    std::chrono::high_resolution_clock::time_point last_frame_time;
};

static GameContext* g_game_context = nullptr;

// ============================================================================
// INITIALIZATION
// ============================================================================
bool Initialize() {
    std::cout << "==============================================\n";
    std::cout << "  🌀 PARADOX PROTOCOL - Initializing...\n";
    std::cout << "==============================================\n";

    g_game_context = new GameContext();

    // ========================================================================
    // LAYER 4: Platform (SDL2 Window)
    // ========================================================================
    std::cout << "[Platform] Initializing SDL2 window...\n";
    platform::Window::Config window_config;
    window_config.title = "Paradox Protocol - Pre-Alpha";
    window_config.width = 1280;
    window_config.height = 720;

    if (!g_game_context->window.Initialize(window_config)) {
        std::cerr << "❌ Failed to initialize SDL2 window\n";
        return false;
    }
    std::cout << "✅ SDL2 window created (1280x720)\n";

    // ========================================================================
    // LAYER 3: View (RmlUi)
    // ========================================================================
    std::cout << "[View] Initializing RmlUi context...\n";
    if (!g_game_context->rmlui.Initialize()) {
        std::cerr << "❌ Failed to initialize RmlUi\n";
        return false;
    }
    std::cout << "✅ RmlUi initialized\n";

    // ========================================================================
    // LAYER 2: Bridge (Event System)
    // ========================================================================
    std::cout << "[Bridge] Setting up UI throttle...\n";
    g_game_context->ui_throttle.SetCallback([]() {
        // This callback fires every 150ms to update UI strings
        // For now, just log to console
        auto credits = g_game_context->game_state.GetResource("credits");
        std::cout << "💰 Credits: " << credits.ToString() << "\n";
    });
    std::cout << "✅ UI throttle configured (150ms interval)\n";

    // ========================================================================
    // LAYER 1: Core (Game State)
    // ========================================================================
    std::cout << "[Core] Initializing game state...\n";

    // Set up initial resources
    g_game_context->game_state.SetResource("credits", core::BigNumber(100));

    // Set up tick callback
    g_game_context->tick_system.SetTickCallback([](float delta_time) {
        // Generate 1 credit per tick (60 per second)
        auto current = g_game_context->game_state.GetResource("credits");
        g_game_context->game_state.SetResource("credits", current + core::BigNumber(1));
    });

    std::cout << "✅ Game state initialized\n";

    // ========================================================================
    // READY
    // ========================================================================
    std::cout << "\n🚀 Initialization complete! Starting main loop...\n";
    std::cout << "==============================================\n\n";

    g_game_context->last_frame_time = std::chrono::high_resolution_clock::now();
    return true;
}

// ============================================================================
// MAIN LOOP (Single Frame)
// ============================================================================
void MainLoopIteration() {
    if (!g_game_context || g_game_context->should_quit) {
        #ifdef __EMSCRIPTEN__
        platform::EmscriptenLoop::CancelMainLoop();
        #endif
        return;
    }

    // ========================================================================
    // FRAME TIMING
    // ========================================================================
    auto current_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> delta_duration = current_time - g_game_context->last_frame_time;
    float delta_time = delta_duration.count();
    g_game_context->last_frame_time = current_time;

    // ========================================================================
    // INPUT
    // ========================================================================
    g_game_context->input.PollEvents(g_game_context->should_quit);

    // ========================================================================
    // GAME LOGIC (Core ticks at 60 Hz)
    // ========================================================================
    g_game_context->tick_system.Update(delta_time);

    // ========================================================================
    // UI UPDATE (Throttled to 150ms)
    // ========================================================================
    g_game_context->ui_throttle.Update(delta_time);

    // ========================================================================
    // RENDER
    // ========================================================================
    g_game_context->window.Clear();

    // RmlUi render (currently just empty context)
    g_game_context->rmlui.Update();
    g_game_context->rmlui.Render();

    g_game_context->window.Present();
}

// ============================================================================
// SHUTDOWN
// ============================================================================
void Shutdown() {
    if (!g_game_context) return;

    std::cout << "\n==============================================\n";
    std::cout << "  🌀 PARADOX PROTOCOL - Shutting down...\n";
    std::cout << "==============================================\n";

    g_game_context->rmlui.Shutdown();
    g_game_context->window.Shutdown();

    delete g_game_context;
    g_game_context = nullptr;

    std::cout << "✅ Shutdown complete\n";
}

// ============================================================================
// MAIN ENTRY POINT
// ============================================================================
int main(int argc, char* argv[]) {
    if (!Initialize()) {
        std::cerr << "❌ Initialization failed\n";
        return 1;
    }

    #ifdef __EMSCRIPTEN__
    // WebAssembly: Use emscripten_set_main_loop
    std::cout << "🌐 Running in WebAssembly mode\n";
    platform::EmscriptenLoop::SetMainLoop(MainLoopIteration);
    #else
    // Desktop: Traditional game loop
    std::cout << "🖥️  Running in Desktop mode\n";
    while (!g_game_context->should_quit) {
        MainLoopIteration();
    }
    #endif

    Shutdown();
    return 0;
}
