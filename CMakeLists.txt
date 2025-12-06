cmake_minimum_required(VERSION 3.20)
project(ParadoxProtocol VERSION 0.1.0 LANGUAGES CXX)

# ============================================================================
# PARADOX PROTOCOL - Root Build Configuration
# ============================================================================
# Cross-platform idle game: iOS, Android, Web (Emscripten), PC
# Architecture: Core (pure C++) → Bridge (events) → View (RmlUi) → Platform (SDL2)
# ============================================================================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================================
# PLATFORM DETECTION
# ============================================================================
if(EMSCRIPTEN)
    message(STATUS "🌐 Building for WebAssembly (Emscripten)")
    set(PLATFORM_WEB TRUE)

    # Emscripten-specific flags
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    add_compile_options(
        -sUSE_SDL=2
        -sALLOW_MEMORY_GROWTH=1
        -sMAXIMUM_MEMORY=4GB
    )
    add_link_options(
        -sUSE_SDL=2
        -sALLOW_MEMORY_GROWTH=1
        -sMAXIMUM_MEMORY=4GB
        --preload-file ${CMAKE_SOURCE_DIR}/data@/data
    )
elseif(IOS)
    message(STATUS "🍎 Building for iOS")
    set(PLATFORM_MOBILE TRUE)
elseif(ANDROID)
    message(STATUS "🤖 Building for Android")
    set(PLATFORM_MOBILE TRUE)
else()
    message(STATUS "🖥️  Building for Desktop")
    set(PLATFORM_DESKTOP TRUE)
endif()

# ============================================================================
# DEPENDENCY MANAGEMENT (FetchContent)
# ============================================================================
include(FetchContent)

# Suppress unnecessary warnings from dependencies
set(FETCHCONTENT_QUIET ON)

# ----------------------------------------------------------------------------
# SDL2 (Windowing, Input, Events)
# ----------------------------------------------------------------------------
if(NOT EMSCRIPTEN)
    message(STATUS "📦 Fetching SDL2...")
    FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-2.28.5  # Stable release
        GIT_SHALLOW TRUE
    )

    # Disable SDL2 tests and examples
    set(SDL2_DISABLE_INSTALL OFF CACHE BOOL "" FORCE)
    set(SDL_TEST OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(SDL2)

    # Create SDL2::SDL2 alias if not exists
    if(NOT TARGET SDL2::SDL2)
        add_library(SDL2::SDL2 ALIAS SDL2)
    endif()
endif()

# ----------------------------------------------------------------------------
# nlohmann_json (JSON serialization)
# ----------------------------------------------------------------------------
message(STATUS "📦 Fetching nlohmann_json...")
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

# ----------------------------------------------------------------------------
# mp++ (Multiprecision arbitrary precision library for BigNumber)
# ----------------------------------------------------------------------------
message(STATUS "📦 Fetching mp++ (arbitrary precision library)...")
FetchContent_Declare(
    mppp
    GIT_REPOSITORY https://github.com/bluescarni/mppp.git
    GIT_TAG v1.0.2
    GIT_SHALLOW TRUE
)

# Configure mp++ to use GMP backend (best performance)
set(MPPP_WITH_QUADMATH OFF CACHE BOOL "" FORCE)
set(MPPP_WITH_MPFR OFF CACHE BOOL "" FORCE)
set(MPPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(MPPP_BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(mppp)

# ----------------------------------------------------------------------------
# RmlUi (User Interface library - HTML/CSS-like for games)
# ----------------------------------------------------------------------------
# NOTE: RmlUi has complex dependencies (FreeType, etc.)
# OPTION 1: Use vcpkg (recommended for production)
#   vcpkg install rmlui sdl2-backend
#
# OPTION 2: FetchContent (manual dependency handling)
message(STATUS "📦 Fetching RmlUi...")

FetchContent_Declare(
    freetype
    GIT_REPOSITORY https://github.com/freetype/freetype.git
    GIT_TAG VER-2-13-2
    GIT_SHALLOW TRUE
)
set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(freetype)

FetchContent_Declare(
    RmlUi
    GIT_REPOSITORY https://github.com/mikke89/RmlUi.git
    GIT_TAG 5.1
    GIT_SHALLOW TRUE
)

set(RMLUI_BACKEND "SDL_GL2" CACHE STRING "" FORCE)
set(BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(RmlUi)

# ----------------------------------------------------------------------------
# Google Test (Unit testing framework)
# ----------------------------------------------------------------------------
message(STATUS "📦 Fetching GoogleTest...")
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
    GIT_SHALLOW TRUE
)

# Prevent GoogleTest from overriding compiler/linker options
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ============================================================================
# COMPILER WARNINGS & FLAGS
# ============================================================================
if(MSVC)
    add_compile_options(/W4 /WX-)  # High warnings, don't treat as errors (for now)
else()
    add_compile_options(
        -Wall -Wextra -Wpedantic
        -Wno-unused-parameter  # Common in game dev
    )
endif()

# ============================================================================
# PROJECT STRUCTURE (Modular CMake)
# ============================================================================
# Each layer is a separate CMake target for clean dependency management

add_subdirectory(src/core)        # LAYER 1: Pure C++ game logic
add_subdirectory(src/bridge)      # LAYER 2: Event system
add_subdirectory(src/view)        # LAYER 3: RmlUi presentation
add_subdirectory(src/platform)    # LAYER 4: SDL2 platform abstraction

# ============================================================================
# MAIN EXECUTABLE
# ============================================================================
add_executable(ParadoxProtocol src/main.cpp)

target_link_libraries(ParadoxProtocol PRIVATE
    Core
    Bridge
    View
    Platform
)

# Copy data directory to build output
add_custom_command(TARGET ParadoxProtocol POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data
        $<TARGET_FILE_DIR:ParadoxProtocol>/data
    COMMENT "Copying data files to build directory..."
)

# ============================================================================
# TESTING
# ============================================================================
enable_testing()
add_subdirectory(tests)

# ============================================================================
# INSTALLATION (Future: Steam, App Stores)
# ============================================================================
install(TARGETS ParadoxProtocol
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION bin
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/data
    DESTINATION bin
)

# ============================================================================
# BUILD INFO
# ============================================================================
message(STATUS "════════════════════════════════════════")
message(STATUS "  🎮 Paradox Protocol Build Configuration")
message(STATUS "════════════════════════════════════════")
message(STATUS "  C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "════════════════════════════════════════")
