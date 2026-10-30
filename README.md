# 🎨 PixLab

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg?logo=c%2B%2B)](https://en.cppreference.com/w/cpp/17)
[![WebAssembly](https://img.shields.io/badge/WebAssembly-WASM-654FF0.svg?logo=webassembly)](https://webassembly.org/)
[![Vite](https://img.shields.io/badge/Vite-Bundler-646CFF.svg?logo=vite)](https://vitejs.dev/)
[![Firebase](https://img.shields.io/badge/Firebase-Auth-FFCA28.svg?logo=firebase)](https://firebase.google.com/)

> **A lightning-fast, privacy-first web image editor powered by C++17 and WebAssembly (WASM).**  
> Transform, filter, and enhance your photos right in your browser — zero server uploads, 100% client-side performance.

---

## 📖 Table of Contents

- [What is PixLab?](#-what-is-pixlab)
- [Why PixLab? (Key Benefits)](#-why-pixlab-key-benefits)
- [Core Features](#-core-features)
- [How It Works (Architecture & Pipeline)](#-how-it-works-architecture--pipeline)
- [Available Filters & Enhancement](#-available-filters--enhancement)
- [Quick Start (Run Locally)](#-quick-start-run-locally)
- [C++ & WebAssembly Compilation](#-c--webassembly-compilation)
- [Project Structure](#-project-structure)
- [Tech Stack](#-tech-stack)
- [Contributing](#-contributing)
- [License](#-license)

---

## 💡 What is PixLab?

**PixLab** is an interactive web-based image editing and enhancement platform. While it looks and feels like a sleek modern web application, the heavy computational work is not done by sluggish JavaScript libraries or distant cloud servers.

Instead, PixLab runs a **compiled C++17 image processing engine directly inside your web browser** using **WebAssembly (WASM)**.

Whether you want to apply classic artistic filters, add retro vignetting, or dramatically clarify and sharpen a soft image using multi-stage spatial unsharp masking, PixLab gives you desktop-grade image processing speed inside any modern browser.

---

## ✨ Why PixLab? (Key Benefits)

- 🔒 **100% Private & Client-Side:** Your photos **never leave your device**. No images are uploaded to any external server or cloud backend for processing.
- ⚡ **Near-Native C++ Speed:** Image transformations are executed using compiled C++ arithmetic algorithms compiled to WebAssembly.
- 🎚️ **Interactive Split-Screen Slider:** Compare the original and filtered images side-by-side in real time with an intuitive swipe handle.
- 📦 **Universal Format Support:** Upload PNG, JPG, JPEG, BMP, or WebP images, and export clean results in PNG, JPG, or BMP.
- 📐 **Aspect-Ratio Aware:** Images retain their native proportions without unwanted stretching, letterboxing, or distortion.
- 🔑 **Cloud-Synced Identity:** Includes built-in Firebase Authentication supporting Google Sign-In and Email/Password credentials.

---

## 🚀 Core Features

| Feature | Description |
|---|---|
| **🎨 6 Pure C++ Filters** | Grayscale, Sepia, 4-Pass Blur, Horizontal Reflection, Pixelate, and Vignette. |
| **⚡ Multi-Stage Enhancer** | High-pass Unsharp Masking combined with an S-curve contrast lookup table and color vibrance lift. |
| **🎛️ Dynamic Intensity Control** | Adjust enhancement strength from 10% to 100% with live re-processing. |
| **↔️ Before & After Comparison** | Interactive slider allows seamless inspection of transformed pixels. |
| **💾 Multi-Format Export** | Download your processed pictures in high-fidelity PNG, JPG, or 24-bit uncompressed BMP. |
| **🔐 User Authentication** | Modal-based user accounts via Firebase (Sign up, Sign in, Google OAuth, Password recovery). |
| **🖼️ Image Gallery** | Dedicated gallery section for browsing and managing your saved edits. |

---

## 🛠️ How It Works (Architecture & Pipeline)

PixLab connects a responsive HTML5/JavaScript frontend with an Object-Oriented C++ engine through WebAssembly linear memory:

```
┌────────────────────────────────────────────────────────────────────────┐
│                          Web Browser (Client)                          │
├────────────────────────────────────────────────────────────────────────┤
│                                                                        │
│   1. Upload Image (PNG / JPG / BMP / WebP)                             │
│         │                                                              │
│         ▼                                                              │
│   2. Offscreen Canvas extracts raw pixel data (RGBA)                   │
│         │                                                              │
│         ▼                                                              │
│   3. JavaScript serializes pixels into in-memory 24-bit BMP binary     │
│         │                                                              │
│         ▼                                                              │
│   4. Allocates buffer in WASM Heap (`Module._malloc`)                  │
│         │                                                              │
│         ▼                                                              │
│   ═══════════════ [ WebAssembly / C++17 Core Engine ] ══════════════   │
│   │                                                                │   │
│   │   • BMPHandler::deserialize() creates Encapsulated Image       │   │
│   │   • FilterFactory::create(filterId, param) returns Filter      │   │
│   │   • filter->apply(Image&) processes pixels in-place            │   │
│   │   • BMPHandler::serialize() converts back to 24-bit BMP bytes  │   │
│   │                                                                │   │
│   ══════════════════════════════════════════════════════════════════   │
│         │                                                              │
│         ▼                                                              │
│   5. JavaScript reads output buffer and releases WASM heap memory      │
│         │                                                              │
│         ▼                                                              │
│   6. Renders to Interactive Split Slider & Multi-format Downloader     │
│                                                                        │
└────────────────────────────────────────────────────────────────────────┘
```

### Why BMP for In-Memory Transfer?
The 24-bit uncompressed BMP format provides a lightweight, deterministic binary format with simple 54-byte headers and BGR scanlines. This enables fast serialization directly between JavaScript `ArrayBuffer`s and C++ `std::vector<uint8_t>` without any third-party image decoding overhead.

---

## 🔬 Available Filters & Enhancement

### 1. Grayscale
Converts color images into balanced monochrome using the **ITU-R Rec. BT.601** perceptual luminance standard:
$$\text{Luminance} = 0.299R + 0.587G + 0.114B$$

### 2. Sepia
Applies a warm, nostalgic antique tone using classic photographic color matrix equations with safe $[0, 255]$ clamping.

### 3. Box Blur (4-Pass Convolution)
Performs four successive iterations of a $3 \times 3$ averaging convolution kernel. Multiple passes approximate a smooth Gaussian blur without heavy trigonometric cost.

### 4. Reflection
Flips the image along its vertical center axis ($Y$-axis horizontal reflection) in $O(W \times H / 2)$ time by swapping opposite pixels across scanlines.

### 5. Pixelate (Mosaic)
Divides the image into distinct blocks (e.g. $10 \times 10$ pixels) and fills each block with its average color, creating a retro 8-bit mosaic effect.

### 6. Vignette
Generates a radial darkening effect from the center toward the corners using a quadratic distance falloff function:
$$\text{factor} = 1.0 - \left(\frac{\text{dist}}{\text{maxDist}}\right)^2 \times 0.75$$

### 7. Multi-Stage Unsharp Mask Image Enhancer
A high-performance 3-stage clarity engine:
1. **Low-Frequency Baseline:** Extracts smooth background tones using an adaptive separable 2D box blur.
2. **High-Pass Sharpening (Unsharp Mask):** Amplifies the fine edge difference $(\text{Original} - \text{Blurred}) \times \alpha$.
3. **Clarity & Vibrance Lift:** Applies a precomputed 256-entry S-curve look-up table (LUT) for micro-contrast alongside a perceptual luminance vibrance boost.

---

## 🏁 Quick Start (Run Locally)

You can run PixLab locally in just a few minutes.

### Prerequisites
Make sure you have [Node.js](https://nodejs.org/) (version 18 or higher) installed on your machine.

### 1. Clone the repository
```bash
git clone https://github.com/Sh4uryaa/PixLab.git
cd PixLab
```

### 2. Install dependencies
```bash
npm install
```

### 3. Start the development server
```bash
npm run dev
```

Once started, open your browser and navigate to the local URL shown in your terminal (usually **`http://localhost:5173`**).

### 4. Build for production
```bash
npm run build
```
The compiled, optimized multi-page production bundle will be generated in the `dist/` directory.

---

## ⚙️ C++ & WebAssembly Compilation

> 💡 **Note:** The repository already includes pre-compiled `pixlab_wasm.js` and `pixlab_wasm.wasm` files, so you **do not** need a C++ compiler just to run or test the web application.

If you want to modify the C++ algorithms or add new filters, follow the steps below:

### Prerequisites for C++ Development
- **GCC / Clang** supporting C++17
- [Emscripten SDK (emsdk)](https://emscripten.org/docs/getting_started/downloads.html) for WebAssembly compilation
- **Make** or **CMake** (version 3.16+)

### 1. Compile C++ to WebAssembly (WASM)
Using Makefile and Emscripten:
```bash
make wasm
```
This compiles the C++ sources in `src/` into `pixlab_wasm.js` and `pixlab_wasm.wasm` with `-O3` optimizations and dynamic memory growth enabled.

### 2. Run Native C++ Unit Tests
Verify image bounds checking, BMP serialization round-trips, and filter math:
```bash
make test
```

### 3. Build Native CLI Executable
Build a desktop executable for standalone testing:
```bash
make cli
./pixlab_cli
```

### Alternative: Building with CMake
```bash
# For native desktop testing:
cmake -B build
cmake --build build

# For WebAssembly build (using Emscripten toolchain):
emcmake cmake -B build-wasm
cmake --build build-wasm
```

---

## 📂 Project Structure

```text
PixLab/
├── index.html              # Main filters workspace with upload and comparison slider
├── enhance.html            # Dedicated unsharp mask image clarity & enhancement page
├── gallery.html            # User image gallery with authentication gating
├── about.html              # About page and project overview
├── 404.html                # Custom error page
├── style.css               # Unified modern dark-themed styling
├── script.js               # Frontend controller, BMP bridge, and WASM memory manager
├── auth.js                 # Firebase Auth integration (Google Sign-In, Email/Password)
├── firebase.js             # Firebase SDK configuration
│
├── include/                # C++ Header Files
│   ├── BMPHandler.h        # 24-bit uncompressed BMP parser & generator
│   ├── Filter.h            # Abstract base class for polymorphic filters
│   ├── FilterFactory.h     # Factory pattern for filter instantiation
│   ├── Image.h             # Encapsulated 2D image matrix and RGBTRIPLE structure
│   ├── GrayscaleFilter.h   # Rec. 601 grayscale filter declaration
│   ├── SepiaFilter.h       # Antique tone filter declaration
│   ├── BlurFilter.h        # 4-pass convolution blur declaration
│   ├── ReflectFilter.h     # Horizontal reflection declaration
│   ├── PixelateFilter.h    # Mosaic pixelation declaration
│   ├── VignetteFilter.h    # Radial vignette declaration
│   └── UnsharpMaskEnhancer.h # Multi-stage clarity and sharpness enhancer declaration
│
├── src/                    # C++ Implementation Files
│   ├── BMPHandler.cpp      # Scanline padding and byte serialization logic
│   ├── Image.cpp           # Pixel buffer management & bounds checking
│   ├── GrayscaleFilter.cpp # Grayscale transformation arithmetic
│   ├── SepiaFilter.cpp     # Sepia matrix math
│   ├── BlurFilter.cpp      # Box blur kernel iterations
│   ├── ReflectFilter.cpp   # Scanline column swapping
│   ├── PixelateFilter.cpp  # Grid averaging algorithm
│   ├── VignetteFilter.cpp  # Quadratic radial falloff
│   ├── UnsharpMaskEnhancer.cpp # Separable blur + high-pass + S-curve LUT
│   └── main.cpp            # WASM exports (_processImage, _getOutputSize, _freeBuffer)
│
├── tests/
│   └── filter_tests.cpp    # Unit test suite verifying all C++ filters & BMP codecs
│
├── public/                 # Static assets for production bundling
│   ├── pixlab_wasm.js      # Emscripten JS runtime loader
│   └── pixlab_wasm.wasm    # Compiled C++ WebAssembly binary
│
├── images/                 # App icons, arrows, and sample comparison images
├── CMakeLists.txt          # CMake build configuration for native & WASM targets
├── Makefile                # Convenient build recipes (wasm, cli, test, clean)
├── package.json            # Vite bundler scripts and npm dependencies
└── vite.config.js          # Multi-page application bundler configuration
```

---

## 💻 Tech Stack

| Domain | Technology | Role |
|---|---|---|
| **Core Engine** | **C++17** | Memory-safe, high-speed image processing algorithms |
| **Runtime Target** | **WebAssembly (WASM)** | Executes native C++ code at near-native speed in the browser |
| **Compiler** | **Emscripten (`emcc`)** | Compiles C++ source into WASM bytecode and JS bridge bindings |
| **Frontend** | **HTML5 / CSS3 / JavaScript (ES6+)** | Responsive user interface, canvas extraction, and drag-and-drop |
| **Dev & Bundling** | **Vite** | Fast Hot-Module-Replacement (HMR) and multi-page production build |
| **Authentication** | **Firebase Auth** | Secure Google OAuth and Email/Password account management |

---

## 🤝 Contributing

Contributions, issues, and feature suggestions are always welcome!

1. **Fork** the repository
2. **Create** your feature branch (`git checkout -b feature/AmazingFilter`)
3. **Commit** your changes (`git commit -m "Add new bilateral filter"`)
4. **Push** to the branch (`git push origin feature/AmazingFilter`)
5. **Open** a Pull Request

---

## 📄 License

This project is licensed under the **MIT License** — see the [LICENSE](LICENSE) file for details.

---

<p align="center">
  Crafted with ❤️ by <b>Shaurya Mishra</b>
</p>