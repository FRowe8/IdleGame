# ============================================================================
# BRIDGE LAYER - Event System & Data Binding
# ============================================================================
# Decouples Core (game logic) from View (RmlUi)
# Implements observer pattern with UI throttling
# ============================================================================

add_library(Bridge STATIC
    EventBus.cpp
    EventBus.h
    UpdateThrottle.cpp
    UpdateThrottle.h
    CommandRouter.cpp
    CommandRouter.h
    DataBindingRegistry.cpp
    DataBindingRegistry.h
)

target_include_directories(Bridge PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(Bridge PUBLIC
    Core
)

target_compile_features(Bridge PUBLIC cxx_std_20)
