# 🔧 Quick Fix for CMake Error

## Error You're Seeing:
```
CMake Error: Could NOT find mp++_GMP (missing: MPPP_GMP_INCLUDE_DIR MPPP_GMP_LIBRARY)
```

## The Problem:
The `mp++` library (used for BigNumber) requires **GMP** (GNU Multiple Precision library), which isn't installed on your system.

---

## ✅ Solution: Install GMP

### Ubuntu/Debian/WSL:
```bash
sudo apt-get update
sudo apt-get install libgmp-dev
```

### macOS (Homebrew):
```bash
brew install gmp
```

###Fedora/RHEL:
```bash
sudo dnf install gmp-devel
```

### Windows (vcpkg):
```bash
vcpkg install gmp
```

---

## After Installing GMP:

```bash
cd IdleGame
rm -rf build  # Clean old build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel 4
```

---

## Verify It Works:
```bash
./ParadoxProtocol
```

You should see:
```
🌀 PARADOX PROTOCOL - Initializing...
✅ SDL2 window + OpenGL context created
✅ RmlUi backends initialized
✅ Game state initialized
🚀 Initialization complete!
```

---

## Alternative: Use vcpkg (if GMP install fails)

**In CLion:**
1. Go to **Settings → Build, Execution, Deployment → CMake**
2. Add CMake option:
   ```
   -DCMAKE_TOOLCHAIN_FILE=[vcpkg path]/scripts/buildsystems/vcpkg.cmake
   ```
3. Run: `vcpkg install gmp sdl2 nlohmann-json rmlui[sdl2-binding,freetype] gtest`
4. Reload CMake Project

vcpkg will handle ALL dependencies automatically!

---

## Still Having Issues?

Paste the **full CMake error output** and I'll help diagnose it.
