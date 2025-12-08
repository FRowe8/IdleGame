# ============================================================================
# CORE LAYER - Pure C++ Game Logic (Zero Dependencies on View/Platform)
# ============================================================================
# This library is 100% testable without SDL2 or RmlUi
# Can run headless for server-side simulation or unit tests
# ============================================================================

add_library(Core STATIC
    # Main game state
    GameState.cpp
    GameState.h

    # Tick system with chunked catch-up
    TickSystem.cpp
    TickSystem.h

    # Arbitrary precision number wrapper
    BigNumber.cpp
    BigNumber.h

    # Game logic modules
    TimeBank.cpp
    TimeBank.h
    ParadoxEngine.cpp
    ParadoxEngine.h
    ResourceManager.cpp
    ResourceManager.h
    PrestigeSystem.cpp
    PrestigeSystem.h

    # Serialization
    serialization/ISerializable.h
    serialization/JsonAdapter.cpp
    serialization/JsonAdapter.h
    serialization/BigNumberConverter.cpp
    serialization/BigNumberConverter.h

    # Events
    events/Event.h
    events/GameEvents.h
)

target_include_directories(Core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(Core PUBLIC
    nlohmann_json::nlohmann_json
)

# Link mp++ only for non-Emscripten builds
if(NOT EMSCRIPTEN)
    target_link_libraries(Core PUBLIC mp++)
    target_compile_definitions(Core PUBLIC PARADOX_USE_MPPP)
else()
    # For Emscripten, use simplified BigNumber (no mp++)
    target_compile_definitions(Core PUBLIC PARADOX_USE_SIMPLE_BIGNUM)
endif()

# Ensure C++20 features
target_compile_features(Core PUBLIC cxx_std_20)
