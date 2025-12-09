cmake_minimum_required(VERSION 3.20)
project(ParadoxProtocol VERSION 0.1.0 LANGUAGES CXX)

# ============================================================================
# PARADOX PROTOCOL - Root Build Configuration
# ============================================================================
# Cross-platform idle game: iOS, Android, Web (Emscripten), PC
# Architecture: Core (pure C++) → Bridge (events) → View (RmlUi) → Platform (SDL2)
#
# DEPENDENCY MANAGEMENT:
# - Prefers vcpkg if available (via find_package)
# - Falls back to FetchContent for missing packages
# ============================================================================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================================
# VCPKG INTEGRATION (Optional)
# ============================================================================
# If using vcpkg via CLion or command line:
#   cmake -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake ..
# ============================================================================

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "🔍 VCPKG_ROOT detected: $ENV{VCPKG_ROOT}")
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

# ============================================================================
# PLATFORM DETECTION
# ============================================================================
if(EMSCRIPTEN)
    message(STATUS "🌐 Building for WebAssembly (Emscripten)")
    set(PLATFORM_WEB TRUE)

    # Emscripten requires static libraries only
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)

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
# DEPENDENCY MANAGEMENT (vcpkg-friendly with FetchContent fallback)
# ============================================================================
include(FetchContent)
set(FETCHCONTENT_QUIET ON)

# ----------------------------------------------------------------------------
# SDL2 (Windowing, Input, Events)
# ----------------------------------------------------------------------------
if(NOT EMSCRIPTEN)
    message(STATUS "📦 Looking for SDL2...")

    # Try vcpkg first
    find_package(SDL2 CONFIG QUIET)

    if(SDL2_FOUND)
        message(STATUS "✅ SDL2 found via vcpkg")
        # vcpkg provides SDL2::SDL2 or SDL2::SDL2-static
        if(NOT TARGET SDL2::SDL2)
            add_library(SDL2::SDL2 ALIAS SDL2::SDL2-static)
        endif()
    else()
        message(STATUS "📥 SDL2 not found, using FetchContent...")
        FetchContent_Declare(
            SDL2
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG release-2.28.5
            GIT_SHALLOW TRUE
        )
        set(SDL2_DISABLE_INSTALL OFF CACHE BOOL "" FORCE)
        set(SDL_TEST OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(SDL2)

        if(NOT TARGET SDL2::SDL2)
            add_library(SDL2::SDL2 ALIAS SDL2)
        endif()
    endif()
endif()

# ----------------------------------------------------------------------------
# nlohmann_json (JSON serialization)
# ----------------------------------------------------------------------------
message(STATUS "📦 Looking for nlohmann_json...")

find_package(nlohmann_json CONFIG QUIET)

if(nlohmann_json_FOUND)
    message(STATUS "✅ nlohmann_json found via vcpkg")
else()
    message(STATUS "📥 nlohmann_json not found, using FetchContent...")
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# ----------------------------------------------------------------------------
# GMP (GNU Multiple Precision Library - required for mp++)
# ----------------------------------------------------------------------------
if(NOT EMSCRIPTEN)
    message(STATUS "📦 Looking for GMP...")

    # Try vcpkg first (find_package with CONFIG mode)
    find_package(GMP CONFIG QUIET)

    if(NOT GMP_FOUND)
        # Try alternative package names that vcpkg might use
        find_package(gmp CONFIG QUIET)
    endif()

    if(NOT GMP_FOUND)
        # Try system installation (using pkg-config)
        find_package(PkgConfig QUIET)
        if(PkgConfig_FOUND)
            pkg_check_modules(GMP QUIET gmp)
            if(GMP_FOUND)
                # Create imported target from pkg-config results
                add_library(GMP::gmp INTERFACE IMPORTED)
                set_target_properties(GMP::gmp PROPERTIES
                    INTERFACE_INCLUDE_DIRECTORIES "${GMP_INCLUDE_DIRS}"
                    INTERFACE_LINK_LIBRARIES "${GMP_LIBRARIES}"
                )
            endif()
        endif()
    endif()

    if(NOT GMP_FOUND AND WIN32)
        # Last resort for Windows: look for GMP manually in common vcpkg locations
        if(DEFINED ENV{VCPKG_ROOT})
            set(VCPKG_INSTALLED "$ENV{VCPKG_ROOT}/installed")
            if(CMAKE_SIZEOF_VOID_P EQUAL 8)
                set(VCPKG_TRIPLET "x64-windows")
            else()
                set(VCPKG_TRIPLET "x86-windows")
            endif()

            find_path(GMP_INCLUDE_DIR
                NAMES gmp.h
                PATHS "${VCPKG_INSTALLED}/${VCPKG_TRIPLET}/include"
                NO_DEFAULT_PATH
            )

            find_library(GMP_LIBRARY
                NAMES gmp
                PATHS "${VCPKG_INSTALLED}/${VCPKG_TRIPLET}/lib"
                NO_DEFAULT_PATH
            )

            if(GMP_INCLUDE_DIR AND GMP_LIBRARY)
                message(STATUS "✅ Found GMP manually in vcpkg: ${GMP_LIBRARY}")
                add_library(GMP::gmp UNKNOWN IMPORTED)
                set_target_properties(GMP::gmp PROPERTIES
                    IMPORTED_LOCATION "${GMP_LIBRARY}"
                    INTERFACE_INCLUDE_DIRECTORIES "${GMP_INCLUDE_DIR}"
                )
                set(GMP_FOUND TRUE)
            endif()
        endif()
    endif()

    if(GMP_FOUND)
        message(STATUS "✅ GMP found")

        # Ensure we have a GMP::gmp target
        if(NOT TARGET GMP::gmp)
            if(TARGET gmp)
                add_library(GMP::gmp ALIAS gmp)
            endif()
        endif()
    else()
        message(STATUS "")
        message(STATUS "❌ GMP (GNU Multiple Precision library) is required for mp++")
        message(STATUS "")

        if(WIN32)
            message(STATUS "📋 Windows Installation Instructions:")
            message(STATUS "")
            message(STATUS "  1. Install vcpkg if you haven't already:")
            message(STATUS "     git clone https://github.com/microsoft/vcpkg.git")
            message(STATUS "     cd vcpkg && .\\bootstrap-vcpkg.bat")
            message(STATUS "")
            message(STATUS "  2. Install GMP:")
            message(STATUS "     .\\vcpkg install gmp:x64-windows")
            message(STATUS "")
            message(STATUS "  3. Set VCPKG_ROOT environment variable:")
            message(STATUS "     setx VCPKG_ROOT \"C:\\path\\to\\vcpkg\"")
            message(STATUS "")
            message(STATUS "  4. Reconfigure CMake with vcpkg toolchain:")
            message(STATUS "     In Visual Studio: Set CMAKE_TOOLCHAIN_FILE in CMakeSettings.json")
            message(STATUS "     Or via command line:")
            message(STATUS "     cmake -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake ..")
            message(STATUS "")
        else()
            message(STATUS "To install GMP:")
            message(STATUS "  vcpkg:         vcpkg install gmp")
            message(STATUS "  Ubuntu/Debian: sudo apt-get install libgmp-dev")
            message(STATUS "  macOS:         brew install gmp")
            message(STATUS "  Fedora:        sudo dnf install gmp-devel")
            message(STATUS "")
            message(STATUS "If using vcpkg, make sure CMake is configured with:")
            message(STATUS "  -DCMAKE_TOOLCHAIN_FILE=[vcpkg]/scripts/buildsystems/vcpkg.cmake")
        endif()

        message(STATUS "")
        message(FATAL_ERROR "GMP not found. Please install it and try again.")
    endif()
else()
    # Emscripten builds: Skip GMP requirement
    # We'll use a JavaScript BigInt-based implementation for WebAssembly
    message(STATUS "🌐 Skipping GMP for WebAssembly build (will use JavaScript BigInt fallback)")
    set(GMP_FOUND FALSE)
endif()

# ----------------------------------------------------------------------------
# mp++ (Multiprecision arbitrary precision library for BigNumber)
# ----------------------------------------------------------------------------
if(NOT EMSCRIPTEN)
    message(STATUS "📦 Looking for mp++...")

    # Try to find mp++ via vcpkg
    find_package(mp++ CONFIG QUIET)

    if(mp++_FOUND)
        message(STATUS "✅ mp++ found via vcpkg")
    else()
        message(STATUS "📥 mp++ not found, using FetchContent...")

        # Fetch mp++ (GMP is already found above)
        FetchContent_Declare(
            mppp
            GIT_REPOSITORY https://github.com/bluescarni/mppp.git
            GIT_TAG v1.0.2
            GIT_SHALLOW TRUE
        )
        set(MPPP_WITH_QUADMATH OFF CACHE BOOL "" FORCE)
        set(MPPP_WITH_MPFR OFF CACHE BOOL "" FORCE)
        set(MPPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        set(MPPP_BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(mppp)
    endif()
else()
    # Emscripten builds: Skip mp++ (will use simplified BigNumber)
    message(STATUS "🌐 Skipping mp++ for WebAssembly build")
endif()

# ----------------------------------------------------------------------------
# RmlUi (User Interface library - HTML/CSS-like for games)
# ----------------------------------------------------------------------------
message(STATUS "📦 Looking for RmlUi...")

find_package(RmlUi CONFIG QUIET)

if(RmlUi_FOUND)
    message(STATUS "✅ RmlUi found via vcpkg")
    # vcpkg provides RmlUi::RmlUi or RmlCore/RmlDebugger
    if(NOT TARGET RmlCore)
        message(FATAL_ERROR "RmlCore target not found. Install with: vcpkg install rmlui[sdl2-binding,freetype]")
    endif()
else()
    message(STATUS "📥 RmlUi not found, using FetchContent...")

    # FreeType dependency
    find_package(Freetype QUIET)
    if(NOT Freetype_FOUND)
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
    endif()

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
endif()

# ----------------------------------------------------------------------------
# Google Test (Unit testing framework)
# ----------------------------------------------------------------------------
message(STATUS "📦 Looking for GoogleTest...")

find_package(GTest CONFIG QUIET)

if(GTest_FOUND)
    message(STATUS "✅ GoogleTest found via vcpkg")
else()
    message(STATUS "📥 GoogleTest not found, using FetchContent...")
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
        GIT_SHALLOW TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# ============================================================================
# COMPILER WARNINGS & FLAGS
# ============================================================================
if(MSVC)
    add_compile_options(/W4 /WX-)
else()
    add_compile_options(
        -Wall -Wextra -Wpedantic
        -Wno-unused-parameter
    )
endif()

# ============================================================================
# PROJECT STRUCTURE (Modular CMake)
# ============================================================================
add_subdirectory(src/core)
add_subdirectory(src/bridge)
add_subdirectory(src/view)
add_subdirectory(src/platform)

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

# Emscripten-specific configuration
if(EMSCRIPTEN)
    set_target_properties(ParadoxProtocol PROPERTIES
        SUFFIX ".js"
        LINK_FLAGS "-sUSE_SDL=2 -sUSE_FREETYPE=1 -sALLOW_MEMORY_GROWTH=1 -sMAXIMUM_MEMORY=4GB -sFULL_ES2=1 -sUSE_WEBGL2=1 --preload-file ${CMAKE_SOURCE_DIR}/data@/data -sEXPORTED_RUNTIME_METHODS=['ccall','cwrap'] -sEXPORTED_FUNCTIONS=['_main'] -sMODULARIZE=0"
    )

    # Copy HTML shell to build directory
    configure_file(
        ${CMAKE_SOURCE_DIR}/web/index.html
        ${CMAKE_BINARY_DIR}/index.html
        COPYONLY
    )
endif()

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
# INSTALLATION
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
if(DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "  vcpkg: ENABLED")
else()
    message(STATUS "  vcpkg: Not detected (using FetchContent)")
endif()
message(STATUS "════════════════════════════════════════")
