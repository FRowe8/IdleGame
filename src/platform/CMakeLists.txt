# ============================================================================
# PLATFORM LAYER - SDL2 Abstraction
# ============================================================================
# Isolates platform-specific code (windowing, input, file I/O)
# ============================================================================

add_library(Platform STATIC
    Window.cpp
    Window.h
    Input.cpp
    Input.h
    FileSystem.cpp
    FileSystem.h
)

# Emscripten-specific code
if(EMSCRIPTEN)
    target_sources(Platform PRIVATE
        emscripten/EmscriptenLoop.cpp
        emscripten/EmscriptenLoop.h
    )
endif()

target_include_directories(Platform PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

if(EMSCRIPTEN)
    # SDL2 is provided by Emscripten via -sUSE_SDL=2
    target_compile_options(Platform PUBLIC -sUSE_SDL=2)
else()
    target_link_libraries(Platform PUBLIC SDL2::SDL2)
endif()

target_compile_features(Platform PUBLIC cxx_std_20)
