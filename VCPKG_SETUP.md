# Building with vcpkg (CLion Integration)

## 🔧 **Setup Instructions**

### **Option 1: CLion Automatic vcpkg Integration** (Recommended)

1. **Install vcpkg** (if not already installed):
   ```bash
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   ./bootstrap-vcpkg.sh  # Linux/Mac
   # OR
   .\bootstrap-vcpkg.bat  # Windows
   ```

2. **Open Project in CLion**:
   - CLion will detect `vcpkg.json` automatically
   - Go to **Settings → Build, Execution, Deployment → CMake**
   - CLion should show: "vcpkg integration: Enabled"

3. **Configure CMake**:
   - CLion will automatically add the toolchain file
   - Click **Reload CMake Project**

4. **Build**:
   ```
   Build → Build Project (Ctrl+F9)
   ```

---

### **Option 2: Manual vcpkg Setup**

1. **Set vcpkg toolchain file** in CMake options:
   ```bash
   cmake -DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake ..
   ```

2. **Or set environment variable**:
   ```bash
   export VCPKG_ROOT=/path/to/vcpkg
   cmake ..
   ```

---

## 📦 **What Gets Installed**

vcpkg will automatically install:
- ✅ **SDL2** (windowing, input)
- ✅ **nlohmann-json** (JSON serialization)
- ✅ **RmlUi** (UI framework) with SDL2 + FreeType
- ✅ **GoogleTest** (unit testing)
- ⚠️ **mp++** - Not in vcpkg by default, will use FetchContent

---

## 🐛 **Troubleshooting**

### **Error: mp++ not found**
This is **expected**. The CMakeLists.txt will fall back to FetchContent for mp++.

You'll see:
```
📦 Looking for mp++...
📥 mp++ not found, using FetchContent...
✅ mp++ downloaded and built
```

### **Error: RmlUi target not found**
Install with SDL2 features:
```bash
vcpkg install rmlui[sdl2-binding,freetype]
```

### **Error: Multiple SDL2 definitions**
vcpkg is providing SDL2, but FetchContent is also trying to download it.
Solution: Already handled in the updated CMakeLists.txt (uses `find_package` first)

---

## 🚀 **Build Verification**

After CLion configures, you should see in the CMake output:
```
✅ SDL2 found via vcpkg
✅ nlohmann_json found via vcpkg
✅ RmlUi found via vcpkg
✅ GoogleTest found via vcpkg
📥 mp++ not found, using FetchContent...
════════════════════════════════════════
  🎮 Paradox Protocol Build Configuration
════════════════════════════════════════
  vcpkg: ENABLED
════════════════════════════════════════
```

---

## ⚙️ **CLion-Specific Settings**

### **Enable vcpkg in CLion**:
1. **Settings → Build, Execution, Deployment → CMake**
2. **CMake options**: (should auto-populate)
   ```
   -DCMAKE_TOOLCHAIN_FILE=[vcpkg path]/scripts/buildsystems/vcpkg.cmake
   ```

3. **Reload CMake Project**

### **Disable FetchContent (optional)**:
If you want to **force** vcpkg-only (no FetchContent fallback):
```cmake
# Add to CMakeLists.txt after project()
set(FETCHCONTENT_FULLY_DISCONNECTED ON)
```

---

## 📝 **Notes**

- **vcpkg manifest mode**: The `vcpkg.json` file declares dependencies
- **Hybrid approach**: CMakeLists.txt tries vcpkg first, falls back to FetchContent
- **mp++ exception**: Not in vcpkg, always uses FetchContent (this is fine)
- **No conflicts**: The updated CMakeLists.txt handles both vcpkg and FetchContent gracefully

---

## ✅ **What This Gives You**

**With vcpkg:**
- ✅ Faster builds (pre-compiled binaries)
- ✅ CLion integration (better IDE support)
- ✅ Consistent versions across team
- ✅ No internet required after initial install

**Fallback to FetchContent:**
- ✅ Works offline (if packages not in vcpkg)
- ✅ No vcpkg setup required
- ✅ Always builds from source

**Best of both worlds!** 🎉
