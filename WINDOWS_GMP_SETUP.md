# 🪟 Windows Build Guide - GMP Setup

## Quick Fix for "GMP not found" Error

If you're seeing `CMake Error at CMakeLists.txt:232 (message): GMP not found`, follow these steps:

---

## ✅ Solution 1: Using vcpkg (Recommended)

### **Step 1: Install vcpkg (if not already installed)**

Open **PowerShell** or **Command Prompt** and run:

```powershell
# Navigate to a location where you want vcpkg (e.g., C:\dev\)
cd C:\dev

# Clone vcpkg
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg

# Bootstrap vcpkg
.\bootstrap-vcpkg.bat

# Integrate with Visual Studio (optional but recommended)
.\vcpkg integrate install
```

### **Step 2: Set VCPKG_ROOT Environment Variable**

```powershell
# Set permanently (requires restarting Visual Studio/terminal)
setx VCPKG_ROOT "C:\dev\vcpkg"

# Or set for current session only
$env:VCPKG_ROOT = "C:\dev\vcpkg"
```

### **Step 3: Install GMP**

```powershell
cd C:\dev\vcpkg

# For 64-bit (recommended)
.\vcpkg install gmp:x64-windows

# Or for 32-bit
.\vcpkg install gmp:x86-windows
```

This will download and build GMP (~2-5 minutes).

### **Step 4: Configure CMake with vcpkg**

#### **Option A: Visual Studio (Automatic)**

If you ran `vcpkg integrate install`, Visual Studio will automatically detect vcpkg packages. Just reload your project and it should work!

#### **Option B: CMake Command Line**

```powershell
cd C:\Users\Computer\Documents\newidlegame\IdleGame
mkdir build
cd build

cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\dev\vcpkg\scripts\buildsystems\vcpkg.cmake -G "Visual Studio 17 2022" -A x64
```

#### **Option C: CMakeSettings.json (Visual Studio)**

Create or edit `.vs/CMakeSettings.json` in your project:

```json
{
  "configurations": [
    {
      "name": "x64-Debug",
      "generator": "Ninja",
      "configurationType": "Debug",
      "buildRoot": "${projectDir}\\out\\build\\${name}",
      "installRoot": "${projectDir}\\out\\install\\${name}",
      "cmakeCommandArgs": "",
      "buildCommandArgs": "",
      "ctestCommandArgs": "",
      "cmakeToolchain": "C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake",
      "variables": []
    },
    {
      "name": "x64-Release",
      "generator": "Ninja",
      "configurationType": "Release",
      "buildRoot": "${projectDir}\\out\\build\\${name}",
      "installRoot": "${projectDir}\\out\\install\\${name}",
      "cmakeCommandArgs": "",
      "buildCommandArgs": "",
      "ctestCommandArgs": "",
      "cmakeToolchain": "C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake",
      "variables": []
    }
  ]
}
```

**Important**: Change `C:/dev/vcpkg` to your actual vcpkg path.

### **Step 5: Rebuild**

Delete your `build/` or `out/` directory and reconfigure:

```powershell
# Clean build
Remove-Item -Recurse -Force build
mkdir build
cd build

# Configure
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\dev\vcpkg\scripts\buildsystems\vcpkg.cmake

# Build
cmake --build . --config Release
```

---

## ✅ Solution 2: Using vcpkg.json (Manifest Mode)

The project already has `vcpkg.json` which should auto-install GMP, but you need to enable manifest mode:

### **Step 1: Ensure vcpkg.json is in project root**

The file should already exist at `C:\Users\Computer\Documents\newidlegame\IdleGame\vcpkg.json`

### **Step 2: Set vcpkg toolchain in CMake**

Visual Studio should detect this automatically if you have vcpkg integrated. If not, add to your `CMakeSettings.json`:

```json
{
  "configurations": [
    {
      "name": "x64-Debug",
      "cmakeToolchain": "C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake",
      "variables": [
        {
          "name": "VCPKG_MANIFEST_MODE",
          "value": "ON"
        }
      ]
    }
  ]
}
```

### **Step 3: Reload project**

Visual Studio will automatically install all dependencies from `vcpkg.json` including GMP!

---

## 🔍 Troubleshooting

### **Error: "Could not find vcpkg"**

Make sure:
1. `VCPKG_ROOT` environment variable is set
2. Restart Visual Studio after setting environment variable
3. Check path with: `echo %VCPKG_ROOT%` in Command Prompt

### **Error: "GMP found but mp++ fails to link"**

Try installing both GMP and mpir:

```powershell
.\vcpkg install gmp:x64-windows mpir:x64-windows
```

### **Error: "Cannot open include file: 'gmp.h'"**

CMake found GMP but Visual Studio can't find headers. Check:

```powershell
# Verify GMP is installed
.\vcpkg list | findstr gmp

# Should show:
# gmp:x64-windows       6.3.0   ...
```

If not installed, run:
```powershell
.\vcpkg install gmp:x64-windows --recurse
```

### **Error: Visual Studio says "Unable to start program"**

This is usually unrelated to GMP. Make sure:
1. Build succeeded (check Output window)
2. SDL2.dll is in the same directory as the .exe
3. You're running the correct startup project

### **Still getting "GMP not found"?**

Run CMake with verbose output to see what's happening:

```powershell
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\dev\vcpkg\scripts\buildsystems\vcpkg.cmake --trace-expand > cmake_trace.txt
```

Then search `cmake_trace.txt` for "GMP" to see why it's failing.

---

## 📊 Verification

After configuring, you should see this in CMake output:

```
-- 📦 Looking for GMP...
-- ✅ GMP found
-- 📦 Looking for mp++...
-- ✅ mp++ found via FetchContent
```

If you see ❌ instead of ✅, the steps above didn't work. Please share:
1. Your vcpkg path
2. Output of `vcpkg list | findstr gmp`
3. CMake configure output (copy the full output)

---

## 🚀 Quick Command Summary

```powershell
# 1. Install vcpkg (one time)
git clone https://github.com/microsoft/vcpkg.git C:\dev\vcpkg
cd C:\dev\vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install

# 2. Set environment variable
setx VCPKG_ROOT "C:\dev\vcpkg"

# 3. Install GMP
.\vcpkg install gmp:x64-windows

# 4. Build project
cd C:\Users\Computer\Documents\newidlegame\IdleGame
cmake -B build -DCMAKE_TOOLCHAIN_FILE=C:\dev\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake --build build --config Release

# 5. Run
.\build\Release\ParadoxProtocol.exe
```

---

## 💡 Alternative: Pre-built GMP (Advanced)

If vcpkg isn't working, you can manually download GMP:

1. Download MPIR (GMP-compatible for Windows): https://github.com/wbhart/mpir/releases
2. Extract to `C:\Libraries\mpir`
3. Set CMake variable: `-DGMP_ROOT=C:\Libraries\mpir`

**Note**: This is more complex and vcpkg is recommended.

---

## 📞 Need More Help?

If you're still stuck, please provide:

1. **Windows version**: `winver` output
2. **Visual Studio version**: Help → About Microsoft Visual Studio
3. **CMake version**: `cmake --version`
4. **vcpkg location**: `echo %VCPKG_ROOT%`
5. **GMP installation status**: `vcpkg list | findstr gmp`
6. **Full CMake error output** (copy from CMake configure step)

This will help diagnose the exact issue!
