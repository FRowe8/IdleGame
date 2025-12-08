#!/bin/bash
# Quick CMake diagnostic script

echo "🔍 Checking CMake environment..."
echo ""

# Check CMake version
echo "CMake version:"
cmake --version | head -1

# Check for vcpkg
if [ -n "$VCPKG_ROOT" ]; then
    echo "✅ vcpkg detected at: $VCPKG_ROOT"
else
    echo "⚠️  vcpkg not detected (will use FetchContent)"
fi

# Check compiler
echo ""
echo "C++ Compiler:"
c++ --version | head -1

# Try minimal build
echo ""
echo "🔧 Testing minimal CMake configuration..."
cd /home/user/IdleGame
rm -rf build-test
mkdir build-test
cd build-test

# Run CMake with verbose output
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE=ON 2>&1 | tee cmake-output.log

if [ $? -eq 0 ]; then
    echo ""
    echo "✅ CMake configuration SUCCESS"
    echo ""
    echo "📦 Dependencies found:"
    grep "found via" cmake-output.log || echo "  All using FetchContent"
else
    echo ""
    echo "❌ CMake configuration FAILED"
    echo ""
    echo "Last 30 lines of output:"
    tail -30 cmake-output.log
fi
