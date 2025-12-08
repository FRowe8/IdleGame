# 🎯 Building with Visual Studio + vcpkg

Visual Studio has **automatic vcpkg manifest mode** support - it reads `vcpkg.json` and installs dependencies for you!

---

## ✅ Quick Setup (3 Steps)

### **Step 1: Install vcpkg Integration**

Open **Developer PowerShell** (or Developer Command Prompt) and run:

```powershell
cd C:\path\to\vcpkg
.\vcpkg integrate install
```

You should see:
```
Applied user-wide integration for this vcpkg root.
CMake projects should use: "-DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake"

All MSBuild C++ projects can now #include any installed libraries.
Linking will be handled automatically.
```

---

### **Step 2: Open Project in Visual Studio**

1. **File → Open → Folder...**
2. Select the `IdleGame` folder
3. Visual Studio will detect `CMakeLists.txt` and `vcpkg.json`

**VS will automatically:**
- Read `vcpkg.json`
- Install all dependencies (gmp, sdl2, rmlui, etc.)
- Configure CMake with the vcpkg toolchain

---

### **Step 3: Build**

1. Wait for CMake to finish configuring (watch the **Output** window)
2. Select **x64-Debug** or **x64-Release** from the configuration dropdown
3. Click **Build → Build All** (Ctrl+Shift+B)

---

## 📊 What You'll See

### **First Time Setup (Installing Dependencies):**

In the **Output** window:
```
-- Running vcpkg install - done
-- Running vcpkg install - resulted in 5 new package(s)
Installing gmp:x64-windows...
Installing sdl2:x64-windows...
Installing nlohmann-json:x64-windows...
Installing rmlui[freetype,sdl2-binding]:x64-windows...
Installing gtest:x64-windows...
```

This might take **5-10 minutes** the first time.

---

### **CMake Configuration Output:**

```
📦 Looking for GMP...
✅ GMP found
📦 Looking for mp++...
📥 mp++ not found, using FetchContent...
📦 Looking for SDL2...
✅ SDL2 found via vcpkg
📦 Looking for nlohmann_json...
✅ nlohmann_json found via vcpkg
📦 Looking for RmlUi...
✅ RmlUi found via vcpkg
📦 Looking for GoogleTest...
✅ GoogleTest found via vcpkg

════════════════════════════════════════
  🎮 Paradox Protocol Build Configuration
════════════════════════════════════════
  C++ Standard: C++20
  Build Type: Release
  Platform: Windows
  vcpkg: ENABLED
════════════════════════════════════════

-- Configuring done
-- Generating done
```

---

### **Build Output:**

```
1>------ Build started: Project: ParadoxProtocol, Configuration: x64-Release ------
1>Building Custom Rule C:/path/to/IdleGame/CMakeLists.txt
1>BigNumber.cpp
1>GameState.cpp
...
1>ParadoxProtocol.exe
1>Build succeeded.
```

---

## 🎮 Running the Game

### **Option 1: From Visual Studio**

1. Right-click **ParadoxProtocol.exe** in Solution Explorer
2. Select **Set as Startup Item**
3. Press **F5** (Debug) or **Ctrl+F5** (Run without debugging)

### **Option 2: From File Explorer**

Navigate to:
```
IdleGame\out\build\x64-Release\
```

Double-click `ParadoxProtocol.exe`

---

## 🐛 Troubleshooting

### **Error: "vcpkg not found"**

**Symptom:** VS doesn't automatically install dependencies

**Fix:**
```powershell
# Run this in Developer PowerShell
cd C:\path\to\vcpkg
.\vcpkg integrate install
```

Then restart Visual Studio.

---

### **Error: "Could NOT find GMP"**

**Symptom:** vcpkg installed but CMake can't find GMP

**Fix:** Check if vcpkg integration is active:

1. **Tools → Options → CMake → General**
2. Check **"Enable vcpkg integration"** is checked
3. If not using default vcpkg location, set **CMake toolchain file** to:
   ```
   C:\path\to\vcpkg\scripts\buildsystems\vcpkg.cmake
   ```

---

### **Error: Build fails with "cannot open file SDL2.lib"**

**Cause:** Dependencies not installed yet

**Fix:** Delete the `out` folder and let VS reconfigure:
1. Close Visual Studio
2. Delete `IdleGame\out\` folder
3. Reopen Visual Studio
4. Wait for vcpkg to install dependencies (watch Output window)

---

### **vcpkg is installing the wrong architecture**

**Fix:** Make sure you're building for **x64**, not Win32:

1. Configuration dropdown should show **x64-Debug** or **x64-Release**
2. If you see **x86-Debug**, change it to **x64-Debug**

---

## 📁 Visual Studio Folder Structure

After building, you'll have:

```
IdleGame/
├── CMakeLists.txt
├── vcpkg.json
├── src/
├── data/
└── out/                           # VS build output
    ├── build/
    │   └── x64-Release/
    │       ├── ParadoxProtocol.exe   ← Your game!
    │       ├── data/                 ← Data copied here
    │       └── *.lib, *.obj files
    └── install/
```

---

## ⚙️ Advanced: Custom vcpkg Location

If vcpkg is **not** at the default location (`C:\src\vcpkg` or `%USERPROFILE%\vcpkg`):

1. **Tools → Options → CMake → General**
2. Under **CMake toolchain file**, set:
   ```
   C:\your\custom\path\vcpkg\scripts\buildsystems\vcpkg.cmake
   ```

Or edit `CMakeSettings.json`:

```json
{
  "configurations": [
    {
      "name": "x64-Release",
      "generator": "Ninja",
      "configurationType": "Release",
      "buildRoot": "${projectDir}\\out\\build\\${name}",
      "cmakeToolchain": "C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake"
    }
  ]
}
```

---

## ✅ Success Checklist

You'll know it's working when:

- [x] **Output window** shows vcpkg installing packages
- [x] **CMake configuration** shows "✅ GMP found"
- [x] **Build succeeds** with no errors
- [x] **ParadoxProtocol.exe** exists in `out\build\x64-Release\`
- [x] **Running the game** opens a window with console output

---

## 🎯 Expected Console Output

When you run `ParadoxProtocol.exe`:

```
==============================================
  🌀 PARADOX PROTOCOL - Initializing...
==============================================
[Platform] Initializing SDL2 + OpenGL...
✅ SDL2 window + OpenGL context created
[RmlUi] Initializing backends...
✅ RmlUi backends initialized
✅ RmlUi context created
⚠️  Failed to load main_screen.rml (expected - fonts missing)
✅ Data binding configured
✅ Game state initialized

🚀 Initialization complete! Starting main loop...
Press ESC or close window to exit

💰 Credits: 100
💰 Credits: 160
💰 Credits: 220
...
```

A **dark blue-gray window** should appear!

---

## 🚀 Next Steps

Once it builds successfully:

1. **Implement BigNumber formatting** - Make numbers show as "1.23K" instead of "1230"
2. **Add fonts** - Fix the RmlUi font loading warning
3. **Add button handlers** - Make UI interactive
4. **Implement TimeBank** - Compound interest system

---

## 💡 Pro Tips

### **Speed up builds:**
- Use **Ninja** generator (faster than MSBuild)
- Build in **Release mode** for testing (much faster than Debug)

### **Clean rebuild:**
- Delete `out\` folder and reopen project
- Or: **Build → Clean All**

### **View build errors:**
- **View → Error List** (Ctrl+\\, E)
- Double-click errors to jump to source

### **Debug crashes:**
- Press **F5** to run with debugger
- Crashes will break at the error line
- View **Call Stack** and **Locals** windows

---

**VS 2022/2019/2017 all work the same way!** The vcpkg manifest mode is automatic. Just open the folder and build. 🎉
