# 🌐 GitHub Pages Deployment Guide

## Overview

Paradox Protocol can be deployed to **GitHub Pages** and played directly in the browser using WebAssembly (Emscripten)!

The game will be accessible at: `https://FRowe8.github.io/IdleGame/`

---

## 🚀 Enable GitHub Pages (One-Time Setup)

Follow these steps to enable GitHub Pages for automatic deployment:

### 1. **Go to Repository Settings**
   - Navigate to: `https://github.com/FRowe8/IdleGame/settings/pages`
   - Or: Click **Settings** tab → **Pages** (in the left sidebar)

### 2. **Configure Source**
   - **Source**: Select **GitHub Actions** (not "Deploy from a branch")
   - This allows the workflow to deploy automatically

### 3. **Save**
   - GitHub will show a blue banner confirming the configuration

---

## 📦 Automatic Deployment

The deployment is **fully automatic**:

1. **Push to `main` branch**
   ```bash
   git push origin main
   ```

2. **GitHub Actions builds the game**
   - The workflow (`.github/workflows/deploy-gh-pages.yml`) triggers automatically
   - Builds the game with Emscripten (WebAssembly)
   - Takes ~5-10 minutes

3. **Game is deployed**
   - Accessible at: `https://FRowe8.github.io/IdleGame/`
   - Updates within 1-2 minutes after build completes

---

## 🔍 Monitor Deployment

### **Check Build Status**
   - Go to: `https://github.com/FRowe8/IdleGame/actions`
   - Look for the "Deploy to GitHub Pages" workflow
   - Green checkmark = Success ✅
   - Red X = Failed ❌ (check logs for errors)

### **View Logs**
   - Click on the workflow run
   - Expand each step to see detailed output
   - Common issues:
     - Missing dependencies (GMP, Freetype)
     - Linker errors (RmlUi backend compatibility)
     - Memory limits

---

## 🎮 How It Works

### **Build Process**

1. **Emscripten Toolchain**
   - Compiles C++ to WebAssembly (`.wasm`)
   - Generates JavaScript glue code (`.js`)
   - Packages data files (`.data`)

2. **SDL2 & OpenGL → Web**
   - SDL2 maps to HTML5 Canvas
   - OpenGL ES 2.0 → WebGL 2.0
   - Input events forwarded from browser

3. **RmlUi UI → Canvas**
   - RmlUi renders to OpenGL context
   - WebGL displays on HTML5 canvas
   - Maintains 60 FPS in browser

### **Deployed Files**

```
deploy/
├── index.html           # Game launcher page
├── ParadoxProtocol.js   # WebAssembly loader (JavaScript)
├── ParadoxProtocol.wasm # Compiled game code (binary)
├── ParadoxProtocol.data # Packaged data/ directory
└── 404.html             # Redirect to index
```

---

## 🛠️ Emscripten Build Flags

The game is built with these optimizations:

```cmake
-sUSE_SDL=2              # Use Emscripten's SDL2 port
-sUSE_FREETYPE=1         # Use Emscripten's FreeType
-sALLOW_MEMORY_GROWTH=1  # Dynamic memory allocation
-sMAXIMUM_MEMORY=4GB     # Max heap size
-sFULL_ES2=1             # Full OpenGL ES 2.0 support
-sUSE_WEBGL2=1           # WebGL 2.0 rendering
--preload-file data@/data # Embed data/ directory
-O3                      # Maximum optimization
```

---

## ⚙️ Manual Build (Local Testing)

Want to test the WebAssembly build locally?

### **1. Install Emscripten**
```bash
# Clone emsdk
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk

# Install and activate latest
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

### **2. Install Dependencies**
```bash
sudo apt-get install libgmp-dev libfreetype6-dev
```

### **3. Build**
```bash
mkdir -p build-wasm
cd build-wasm

emcmake cmake .. -DCMAKE_BUILD_TYPE=Release
emmake make -j$(nproc)
```

### **4. Run Local Server**
```bash
# Emscripten provides a test server
emrun --no_browser --port 8080 .

# Or use Python
python3 -m http.server 8080
```

### **5. Open in Browser**
```
http://localhost:8080/index.html
```

---

## 🐛 Troubleshooting

### **Build Fails with "GMP not found"**
- **Solution**: GitHub Actions installs `libgmp-dev` automatically
- If building locally: `sudo apt-get install libgmp-dev`

### **Build Fails with "RmlUi backend errors"**
- **Expected**: RmlUi backends have API version mismatches with FetchContent
- **Impact**: May prevent full UI rendering in WebAssembly build
- **Workaround**: Consider using simpler UI or fixing backend compatibility

### **Game Loads but Black Screen**
- **Check console**: Right-click → Inspect → Console tab
- **Common causes**:
  - Missing data files (check `ParadoxProtocol.data`)
  - WebGL not supported (try Chrome/Firefox)
  - OpenGL shader compilation errors

### **"Module is not defined"**
- **Cause**: JavaScript loading order issue
- **Solution**: Ensure `ParadoxProtocol.js` loads after Module object definition

### **Performance Issues**
- **Optimization**: Build with `-O3` flag (already enabled)
- **Memory**: Increase `-sMAXIMUM_MEMORY=4GB` if needed
- **Threading**: Not supported in GitHub Pages (SharedArrayBuffer restrictions)

---

## 📊 Workflow Structure

```yaml
# .github/workflows/deploy-gh-pages.yml

1. Checkout code
2. Setup Emscripten SDK
3. Install system dependencies (GMP, FreeType, OpenGL)
4. Configure CMake with emcmake
5. Build with emmake
6. Copy output files to deploy/
7. Upload artifact
8. Deploy to GitHub Pages
```

---

## 🎯 Expected Deployment Timeline

| Step | Duration |
|------|----------|
| Push to main | Instant |
| Workflow trigger | ~10 seconds |
| Setup & install deps | ~2 minutes |
| CMake configure | ~3 minutes |
| Compile & link | ~5 minutes |
| Deploy to Pages | ~30 seconds |
| **Total** | **~10-12 minutes** |

---

## 🌟 Next Steps

Once deployed, you can:

1. **Share the link**: `https://FRowe8.github.io/IdleGame/`
2. **Test on mobile**: WebAssembly works on mobile browsers
3. **Monitor analytics**: Enable GitHub Pages analytics
4. **Add custom domain**: Configure in Settings → Pages

---

## 📝 Important Notes

- **First deployment**: May take longer (~15 minutes)
- **Subsequent deployments**: Faster (~5-8 minutes) due to caching
- **Build artifacts**: Stored for 90 days in GitHub Actions
- **Browser compatibility**: Chrome, Firefox, Edge, Safari (iOS 11.3+)
- **Mobile support**: Works but may have performance limitations

---

## 🔗 Useful Links

- **Live Game**: https://FRowe8.github.io/IdleGame/
- **Build Status**: https://github.com/FRowe8/IdleGame/actions
- **GitHub Pages Docs**: https://docs.github.com/en/pages
- **Emscripten Docs**: https://emscripten.org/docs/

---

## 🎮 Play Now!

After enabling GitHub Pages and pushing to `main`, your game will be live at:

### **🌐 https://FRowe8.github.io/IdleGame/**

Enjoy your time-traveling idle game! ⏰✨
