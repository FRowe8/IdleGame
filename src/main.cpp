/**
 * PARADOX PROTOCOL - Main Entry Point
 * ====================================
 * Time Travel Resource Arbitrage Idle Game
 * Architecture: Core → Bridge → View → Platform
 */

#include "platform/Window.h"
#include "platform/Input.h"
#ifndef __EMSCRIPTEN__
#include "platform/rmlui_backend/RmlUi_Platform_SDL.h"
#include "platform/rmlui_backend/RmlUi_Renderer_GL2.h"
#endif
#include "view/RmlUiContext.h"
#include "view/DataModelBinder.h"
#include "core/TickSystem.h"
#include "core/GameState.h"
#include "core/BigNumber.h"
#include "bridge/UpdateThrottle.h"
#include "bridge/DataBindingRegistry.h"

#include <iostream>
#include <chrono>

#ifdef __EMSCRIPTEN__
#include "platform/emscripten/EmscriptenLoop.h"
#endif

using namespace paradox;

// ============================================================================
// GLOBAL GAME OBJECTS
// ============================================================================
struct GameContext {
    platform::Window window;
    platform::Input input;
    view::RmlUiContext rmlui;
    view::DataModelBinder data_binder;

    core::GameState game_state;
    core::TickSystem tick_system{60};  // 60 Hz tick rate

    bridge::UpdateThrottle ui_throttle{0.15f};  // Update UI every 150ms
    bridge::DataBindingRegistry binding_registry;

    bool should_quit = false;
    std::chrono::high_resolution_clock::time_point last_frame_time;
};

static GameContext* g_game_context = nullptr;

#ifndef __EMSCRIPTEN__
// Forward declaration (desktop only)
extern RenderInterface_GL2* g_render_interface_gl2;
#endif

// ============================================================================
// INITIALIZATION
// ============================================================================
bool Initialize() {
    std::cout << "==============================================\n";
    std::cout << "  🌀 PARADOX PROTOCOL - Initializing...\n";
    std::cout << "==============================================\n";

    g_game_context = new GameContext();

    // ========================================================================
    // LAYER 4: Platform (SDL2 Window + OpenGL)
    // ========================================================================
    std::cout << "[Platform] Initializing SDL2 + OpenGL...\n";
    platform::Window::Config window_config;
    window_config.title = "Paradox Protocol - First Playable";
    window_config.width = 1280;
    window_config.height = 720;

    if (!g_game_context->window.Initialize(window_config)) {
        std::cerr << "❌ Failed to initialize window\n";
        return false;
    }
    std::cout << "✅ SDL2 window + OpenGL context created\n";

#ifndef __EMSCRIPTEN__
    // ========================================================================
    // RmlUi Backends (Desktop only - Emscripten has built-in SDL/WebGL)
    // ========================================================================
    std::cout << "[RmlUi] Initializing backends...\n";

    // Initialize SDL platform backend
    if (!RmlSDL::Initialize(g_game_context->window.GetSDLWindow())) {
        std::cerr << "❌ Failed to initialize RmlUi SDL backend\n";
        return false;
    }

    // Initialize OpenGL2 renderer
    if (!RmlGL2::Initialize()) {
        std::cerr << "❌ Failed to initialize RmlUi GL2 renderer\n";
        return false;
    }

    // Set viewport for renderer
    int width, height;
    g_game_context->window.GetSize(width, height);
    if (g_render_interface_gl2) {
        g_render_interface_gl2->SetViewport(width, height);
    }

    std::cout << "✅ RmlUi backends initialized\n";
#else
    std::cout << "🌐 Using Emscripten built-in SDL/WebGL (no custom backends needed)\n";
#endif

    // ========================================================================
    // LAYER 3: View (RmlUi Context)
    // ========================================================================
    std::cout << "[View] Initializing RmlUi context...\n";
    if (!g_game_context->rmlui.Initialize()) {
        std::cerr << "⚠️  RmlUi initialized but fonts may be missing\n";
        // Continue anyway - fonts are optional for basic testing
    } else {
        std::cout << "✅ RmlUi context created\n";
    }

    // Load main UI document
    std::cout << "[View] Loading main UI document...\n";
    Rml::ElementDocument* doc = g_game_context->rmlui.LoadDocument("data/ui/main_screen.rml");
    if (!doc) {
        std::cerr << "⚠️  Failed to load main_screen.rml (continuing with empty UI)\n";
    } else {
        std::cout << "✅ UI document loaded\n";
    }

    // ========================================================================
    // LAYER 2: Bridge (Event System & Data Binding)
    // ========================================================================
    std::cout << "[Bridge] Setting up data binding...\n";

    // Initialize data model binder
    g_game_context->data_binder.Initialize(
        g_game_context->rmlui.GetContext(),
        &g_game_context->game_state,
        &g_game_context->binding_registry
    );

    // Set up UI throttle to update data bindings
    g_game_context->ui_throttle.SetCallback([]() {
        g_game_context->data_binder.UpdateBindings();
    });

    std::cout << "✅ Data binding configured\n";

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
    // INPUT FORWARDING
    // ========================================================================
    g_game_context->input.SetRmlUiContext(g_game_context->rmlui.GetContext());

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

    // RmlUi render
#ifndef __EMSCRIPTEN__
    if (g_render_interface_gl2) {
        g_render_interface_gl2->BeginFrame();
    }
#endif
    g_game_context->rmlui.Update();
    g_game_context->rmlui.Render();
#ifndef __EMSCRIPTEN__
    if (g_render_interface_gl2) {
        g_render_interface_gl2->EndFrame();
    }
#endif

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

#ifndef __EMSCRIPTEN__
    RmlGL2::Shutdown();
    RmlSDL::Shutdown();
#endif

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
    std::cout << "Press ESC or close window to exit\n\n";
    while (!g_game_context->should_quit) {
        MainLoopIteration();
    }
    #endif

    Shutdown();
    return 0;
}
