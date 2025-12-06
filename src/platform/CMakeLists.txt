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

    # RmlUi Backend Integration
    rmlui_backend/RmlUi_Platform_SDL.cpp
    rmlui_backend/RmlUi_Platform_SDL.h
    rmlui_backend/RmlUi_Renderer_GL2.cpp
    rmlui_backend/RmlUi_Renderer_GL2.h
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
    ${CMAKE_CURRENT_SOURCE_DIR}/rmlui_backend
)

if(EMSCRIPTEN)
    # SDL2 is provided by Emscripten via -sUSE_SDL=2
    target_compile_options(Platform PUBLIC -sUSE_SDL=2)
else()
    target_link_libraries(Platform PUBLIC SDL2::SDL2)

    # OpenGL for RmlUi renderer (desktop only)
    find_package(OpenGL REQUIRED)
    target_link_libraries(Platform PUBLIC OpenGL::GL)
endif()

# Link RmlUi
target_link_libraries(Platform PUBLIC RmlCore)

target_compile_features(Platform PUBLIC cxx_std_20)
