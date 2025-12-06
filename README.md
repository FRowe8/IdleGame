# ⏱️ Paradox Protocol

**Time Travel Resource Arbitrage Idle Game**

A high-performance, mobile-first idle/incremental game built with modern C++20, featuring a unique time-loop mechanic where players exploit compounding interest across timelines while managing paradox risk.

---

## 🎮 Game Concept

**Core Hook:** You're a temporal economist who can send resources backward in time, creating causal loops for exponential growth—but each transaction increases paradox instability that risks timeline collapse.

**USP Mechanic:** Causal Loops & Paradox Risk
- **Time Bank:** Resources generate compound interest (1% per second)
- **Send Back:** Transfer credits to any previous point in your run
- **Paradox System:** Each time-loop increases instability
  - Low instability: Safe, predictable gains
  - High instability: Risk losing portion of resources

**Prestige System:** Timeline Collapse
- Reset triggers a "Major Paradox" event
- Earn **Temporal Anchors** based on:
  - Successfully closed causal loops
  - Peak instability survived
  - Timeline diversity explored
- Spend anchors on meta-upgrades (interest rate, paradox resistance, timeline slots)

---

## 🏗️ Architecture

### **Layered Hexagonal Architecture**

```
┌─────────────────────────────────────────┐
│     View Layer (RmlUi + SDL2)           │  ← Presentation only
├─────────────────────────────────────────┤
│     Bridge Layer (Event System)         │  ← Decoupling layer
├─────────────────────────────────────────┤
│     Core Layer (Pure C++ Logic)         │  ← Zero dependencies
└─────────────────────────────────────────┘
         Platform Layer (SDL2/Emscripten)    ← OS abstraction
```

**Dependency Rule:** Dependencies flow ONLY downward. Core knows nothing about UI.

---

## 📂 Project Structure

```
IdleGame/
├── CMakeLists.txt                    # Root build config (FetchContent for deps)
├── data/                             # Data-driven content (JSON)
│   ├── balance/                      # Game economy config
│   │   ├── generators.json
│   │   ├── upgrades.json
│   │   └── prestige.json
│   └── ui/                           # RmlUi templates & styles
│       ├── main_screen.rml
│       └── styles/main.rcss
│
├── src/
│   ├── core/                         # 🎯 LAYER 1: Pure C++ game logic
│   │   ├── BigNumber.h/.cpp          # Arbitrary precision (mp++)
│   │   ├── TickSystem.h/.cpp         # ⚙️ Chunked catch-up simulation
│   │   ├── GameState.h/.cpp          # Central state container
│   │   ├── TimeBank.h/.cpp           # Compound interest calculator
│   │   ├── ParadoxEngine.h/.cpp      # Causal loop validator
│   │   ├── serialization/
│   │   │   ├── JsonAdapter.h/.cpp    # nlohmann/json integration
│   │   │   └── BigNumberConverter.h  # 🔥 BigNumber ↔ JSON string
│   │   └── events/                   # Event definitions
│   │
│   ├── bridge/                       # 🌉 LAYER 2: Core ↔ View communication
│   │   ├── EventBus.h/.cpp           # Observer pattern dispatcher
│   │   ├── UpdateThrottle.h/.cpp     # 🔥 UI poll timer (100-200ms)
│   │   ├── CommandRouter.h/.cpp      # UI → Core command routing
│   │   └── DataBindingRegistry.h     # Data model binding
│   │
│   ├── view/                         # 🎨 LAYER 3: RmlUi presentation
│   │   ├── RmlUiContext.h/.cpp       # RmlUi initialization
│   │   ├── ScreenManager.h/.cpp      # Screen transitions
│   │   └── custom_elements/
│   │       ├── TimelineCanvas.h      # 🔥 Visual timeline graph
│   │       └── ParadoxMeter.h        # Instability indicator
│   │
│   ├── platform/                     # 🖥️ LAYER 4: Platform abstraction
│   │   ├── Window.h/.cpp             # SDL2 window wrapper
│   │   ├── Input.h/.cpp              # SDL2 input handling
│   │   ├── FileSystem.h/.cpp         # Cross-platform file I/O
│   │   └── emscripten/               # WebAssembly-specific code
│   │
│   └── main.cpp                      # 🚀 Application entry point
│
└── tests/                            # ✅ Unit tests (Google Test)
    ├── core/
    │   ├── test_BigNumber.cpp
    │   ├── test_TickSystem.cpp       # Test chunked catch-up
    │   └── test_Serialization.cpp    # Test JSON round-trip
    └── test_data/
```

---

## 🛠️ Build Instructions

### **Prerequisites**

- **C++20 Compiler** (GCC 11+, Clang 14+, MSVC 2022+)
- **CMake 3.20+**
- **Git** (for FetchContent dependencies)

> **Note:** All dependencies (SDL2, RmlUi, nlohmann_json, mp++) are fetched automatically via CMake FetchContent.

### **Desktop Build (Windows, macOS, Linux)**

```bash
# Clone repository
git clone https://github.com/YourUsername/IdleGame.git
cd IdleGame

# Create build directory
mkdir build && cd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . --config Release

# Run
./ParadoxProtocol  # (or ParadoxProtocol.exe on Windows)
```

### **Web Build (Emscripten/WebAssembly)**

```bash
# Install Emscripten SDK (if not already installed)
# https://emscripten.org/docs/getting_started/downloads.html

# Activate Emscripten environment
source /path/to/emsdk/emsdk_env.sh

# Configure for Emscripten
mkdir build-web && cd build-web
emcmake cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build .

# Run local server
emrun ParadoxProtocol.html
```

### **iOS / Android**

> Build scripts for mobile platforms coming soon. Architecture is designed to support both.

---

## 🔑 Production-Ready Constraints (Implemented)

### **1. Serialization Strategy**
- **Problem:** JSON cannot store arbitrary precision numbers (e.g., 10^308)
- **Solution:** Custom `BigNumberConverter` serializes `BigNumber` as base-10 **strings**
- **Location:** `src/core/serialization/BigNumberConverter.h`

```cpp
// Automatic serialization via nlohmann/json
json save_data;
save_data["credits"] = BigNumber("999999999999999999999");  // Stored as string
```

### **2. Offline Catch-up**
- **Problem:** Cannot run 86,400 ticks in one frame when player returns after 24 hours
- **Solution:** **Chunked Simulation** in `TickSystem`
  - Process max 5000 ticks per frame
  - Spread catch-up across multiple frames
  - Progress callbacks for loading bar
- **Location:** `src/core/TickSystem.h`

```cpp
tick_system.SimulateOfflineTime(86400.0f);  // 24 hours
while (tick_system.IsCatchingUp()) {
    ui.ShowLoadingBar(tick_system.GetCatchUpProgress());
    tick_system.Update(frame_delta);
}
```

### **3. UI Throttling**
- **Problem:** Core ticks at 60Hz, but updating UI strings every frame wastes battery
- **Solution:** `UpdateThrottle` in Bridge layer
  - Core runs at 60Hz (game logic)
  - UI updates at 5-10Hz (string formatting)
- **Location:** `src/bridge/UpdateThrottle.h`

```cpp
UpdateThrottle ui_throttle(0.15f);  // 150ms interval
ui_throttle.SetCallback([&]() {
    label.SetText(big_number.ToHumanReadable());  // Expensive formatting
});
```

### **4. Visual Timeline**
- **Requirement:** Time-loop mechanic needs visual representation (not just buttons)
- **Solution:** Custom RmlUi element `TimelineCanvas`
  - Renders timeline axis (past → present → future)
  - Draws loop arcs showing resource transfers
  - Visualizes paradox instability
- **Location:** `src/view/custom_elements/TimelineCanvas.h` *(stub for now)*

---

## 🧪 Testing

### **Run Unit Tests**

```bash
cd build
ctest --output-on-failure
```

### **Core is 100% Testable**

All game logic in `src/core/` has ZERO dependencies on SDL2 or RmlUi.
You can write tests without opening a window:

```cpp
TEST(TimeBank, CompoundInterest) {
    TimeBank bank;
    bank.Deposit(BigNumber(100));
    bank.Update(1.0f);  // 1 second
    ASSERT_GT(bank.GetBalance(), BigNumber(100));  // Interest accrued
}
```

---

## 📊 Tech Stack

| Component | Library | Purpose |
|-----------|---------|---------|
| **Build System** | CMake 3.20+ | Cross-platform builds |
| **Language** | C++20 | Modern features (concepts, modules, ranges) |
| **Windowing** | SDL2 | Cross-platform window/input |
| **UI Framework** | RmlUi | HTML/CSS-like game UI |
| **JSON** | nlohmann_json | Data serialization |
| **BigNumber** | mp++ (GMP backend) | Arbitrary precision math |
| **Testing** | Google Test | Unit testing framework |
| **Web Export** | Emscripten | Compile to WebAssembly |

---

## 🚀 Roadmap

### **Phase 1: Core Mechanics** ✅ (Current)
- [x] Architecture setup
- [x] BigNumber system
- [x] Tick system with offline catch-up
- [x] Basic serialization
- [ ] Time Bank implementation
- [ ] Paradox Engine implementation

### **Phase 2: UI Integration**
- [ ] RmlUi data binding
- [ ] Visual timeline canvas
- [ ] Screen management (main game, prestige, settings)
- [ ] Mobile-friendly touch controls

### **Phase 3: Game Content**
- [ ] Generator system (load from JSON)
- [ ] Upgrade tree
- [ ] Prestige system implementation
- [ ] Balance tuning

### **Phase 4: Polish & Optimization**
- [ ] Particle effects
- [ ] Sound system
- [ ] Save/load functionality
- [ ] Performance profiling

### **Phase 5: Platform Releases**
- [ ] Steam (PC)
- [ ] GitHub Pages (Web)
- [ ] iOS App Store
- [ ] Google Play Store

---

## 🤝 Contributing

This is a solo dev project currently, but design feedback is welcome!

**Design Philosophy:**
- **Depth > Grind:** Every mechanic should involve meaningful decisions
- **Mobile-First:** Touch-friendly, battery-efficient
- **Data-Driven:** Balance changes shouldn't require recompiling

---

## 📜 License

MIT License - See LICENSE file for details

---

## 🙏 Acknowledgments

- **RmlUi Team** - Excellent UI library
- **mp++ Developers** - High-performance arbitrary precision
- **Idle Game Community** - Inspiration from games like Antimatter Dimensions, Idle Loops

---

**Built with ⚡ by a frustrated physicist who loves time-travel paradoxes**
