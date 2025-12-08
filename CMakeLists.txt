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
# mp++ (Multiprecision arbitrary precision library for BigNumber)
# ----------------------------------------------------------------------------
message(STATUS "📦 Looking for mp++...")

# Try to find mp++ (vcpkg package name might be 'mppp')
find_package(mp++ CONFIG QUIET)

if(mp++_FOUND)
    message(STATUS "✅ mp++ found via vcpkg")
else()
    message(STATUS "📥 mp++ not found, using FetchContent...")

    # mp++ requires GMP - try to find it first
    find_package(GMP QUIET)

    if(NOT GMP_FOUND)
        message(STATUS "📥 GMP not found, fetching via FetchContent...")

        # Fetch mini-gmp (lightweight GMP alternative that's header-only friendly)
        # OR install system GMP: sudo apt-get install libgmp-dev
        FetchContent_Declare(
            gmp
            URL https://gmplib.org/download/gmp/gmp-6.3.0.tar.xz
            URL_HASH SHA256=a3c2b80201b89e68616f4ad30bc66aee4927c3ce50e33929ca819d5c43538898
        )

        FetchContent_GetProperties(gmp)
        if(NOT gmp_POPULATED)
            message(STATUS "📥 Downloading and building GMP (this may take a few minutes)...")
            FetchContent_Populate(gmp)

            # Build GMP using its configure script
            execute_process(
                COMMAND ${gmp_SOURCE_DIR}/configure --prefix=${CMAKE_BINARY_DIR}/gmp-install --enable-cxx --disable-shared
                WORKING_DIRECTORY ${gmp_BINARY_DIR}
                RESULT_VARIABLE GMP_CONFIGURE_RESULT
                OUTPUT_QUIET ERROR_QUIET
            )

            if(GMP_CONFIGURE_RESULT EQUAL 0)
                execute_process(
                    COMMAND make -j4
                    WORKING_DIRECTORY ${gmp_BINARY_DIR}
                    RESULT_VARIABLE GMP_BUILD_RESULT
                    OUTPUT_QUIET ERROR_QUIET
                )

                if(GMP_BUILD_RESULT EQUAL 0)
                    execute_process(
                        COMMAND make install
                        WORKING_DIRECTORY ${gmp_BINARY_DIR}
                        OUTPUT_QUIET ERROR_QUIET
                    )

                    # Set GMP paths for mp++
                    set(GMP_INCLUDE_DIR "${CMAKE_BINARY_DIR}/gmp-install/include" CACHE PATH "")
                    set(GMP_LIBRARY "${CMAKE_BINARY_DIR}/gmp-install/lib/libgmp.a" CACHE FILEPATH "")
                    set(GMPXX_LIBRARY "${CMAKE_BINARY_DIR}/gmp-install/lib/libgmpxx.a" CACHE FILEPATH "")

                    message(STATUS "✅ GMP built successfully")
                else()
                    message(FATAL_ERROR "❌ GMP build failed. Please install manually: sudo apt-get install libgmp-dev")
                endif()
            else()
                message(FATAL_ERROR "❌ GMP configure failed. Please install manually: sudo apt-get install libgmp-dev")
            endif()
        endif()
    else()
        message(STATUS "✅ GMP found")
    endif()

    # Now fetch mp++
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
