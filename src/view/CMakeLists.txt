# ============================================================================
# VIEW LAYER - RmlUi Presentation
# ============================================================================
# Handles all UI rendering and user interaction
# ============================================================================

add_library(View STATIC
    ScreenManager.cpp
    ScreenManager.h
    RmlUiContext.cpp
    RmlUiContext.h
    DataModelBinder.cpp
    DataModelBinder.h

    custom_elements/TimelineCanvas.cpp
    custom_elements/TimelineCanvas.h
    custom_elements/ParadoxMeter.cpp
    custom_elements/ParadoxMeter.h
)

target_include_directories(View PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(View PUBLIC
    Core
    Bridge
    RmlCore
    RmlDebugger
)

target_compile_features(View PUBLIC cxx_std_20)
