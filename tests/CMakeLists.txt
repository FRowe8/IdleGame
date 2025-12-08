# ============================================================================
# PARADOX PROTOCOL - Test Suite
# ============================================================================

# GoogleTest is already found in root CMakeLists.txt
include(GoogleTest)

# ============================================================================
# CORE TESTS
# ============================================================================
add_executable(CoreTests
    core/BigNumberTest.cpp
)

target_link_libraries(CoreTests PRIVATE
    Core
    GTest::gtest
    GTest::gtest_main
)

# Discover tests
gtest_discover_tests(CoreTests)
