# PixLab: C++ WebAssembly Image Processing Studio

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=c%2B%2B)
![WebAssembly](https://img.shields.io/badge/WebAssembly-WASM-654FF0?logo=webassembly)
![Frontend](https://img.shields.io/badge/Frontend-HTML5%20%2F%20CSS3%20%2F%20ES6%2B-E34F26)
![Build](https://img.shields.io/badge/Build-CMake%20%7C%20Makefile-brightgreen)

**PixLab** is a high-performance, client-side image processing application built with modern **C++17** compiled directly into **WebAssembly (WASM)** via Emscripten. PixLab runs complex spatial convolutions, matrix filters, and multi-stage image enhancement algorithms entirely in browser RAM—with **zero server disk I/O** and **zero machine learning / neural networks**.

---

## 🌟 Key Highlights

- **Pure Client-Side In-Memory Execution**: No images are ever uploaded to an external server or saved to server disk. All pixel transformations execute in the browser heap in RAM.
- **Strict C++ Object-Oriented Architecture (OOP)**: Designed using all four pillars of OOP:
  - **Encapsulation**: `Image` encapsulates flat 24-bit BGR pixel buffers with strict boundary checking.
  - **Abstraction**: Abstract `Filter` base class defining `virtual void apply(Image& img) = 0`.
  - **Inheritance**: Subclassed filters (`GrayscaleFilter`, `SepiaFilter`, `BlurFilter`, `ReflectFilter`, `PixelateFilter`, `VignetteFilter`, `UnsharpMaskEnhancer`).
  - **Polymorphism**: `FilterFactory` dynamic dispatch returning `std::unique_ptr<Filter>`.
- **Zero Machine Learning (Pure Mathematical Convolutions)**: Every effect is computed via deterministic, classical arithmetic algorithms (spatial box convolutions, unsharp masking, S-curve tone curves, and linear color spaces).
- **Interactive Before & After Split Slider**: Real-time dual image comparison with dynamic container aspect ratio matching to preserve the image's original dimensions without cropping or distortion.
- **Multi-Format Upload & Export**: Seamlessly import any standard image format (`PNG`, `JPG`, `JPEG`, `BMP`, `WebP`) via drag-and-drop or file picker, and export processed results in **PNG**, **JPG**, or **24-bit uncompressed Windows BMP**.
- **Adjustable Enhancement Strength Slider**: Fine-tune enhancement intensity from **10% (Gentle)** to **100% (Maximum)** with live re-processing.

---

## 🛠️ Features & Filters

### 1. Filter Studio (`index.html`)
| Filter | Mathematical Technique | Description |
|---|---|---|
| **Grayscale** | ITU-R BT.601 Luminance ($Y = 0.299R + 0.587G + 0.114B$) | Perceptually weighted monochrome conversion |
| **Sepia** | Linear Color Transformation Matrix | Classic warm vintage tone mapping with saturation clamp |
| **Blur** | Separable Multi-Pass Box Convolution ($O(W \times H)$) | Adaptive radius ($R = 3 \sim 20\text{px}$) for smooth, noticeable softening |
| **Reflection** | Horizontal Coordinate Swap ($x \leftrightarrow W - 1 - x$) | Exact horizontal mirror imaging |
| **Pixelate** | $N \times N$ Spatial Block Tessellation | Dynamic spatial decimation and block color averaging |
| **Vignette** | Cosine / Quadratic Radial Falloff ($1 - d^2$) | Soft, circular optical edge darkening |

### 2. Multi-Stage Image Enhancer (`enhance.html`)
The image enhancer provides noticeable clarity, crisp edge separation, and color vibrancy using a four-stage classical mathematical pipeline:
1. **Adaptive Spatial Unsharp Masking**: Extracts high-frequency edge gradients ($\Delta = I_{\text{orig}} - I_{\text{blur}}$) using an adaptive radius ($R = \max(2, \min(6, \lfloor\min(W, H) / 100\rfloor))$).
2. **High-Pass Gain Amplification**: Multiplies edge differences by high-pass gain factor $\alpha = 0.3 + P \times 3.3$, scaled directly from the user's slider percentage $P \in [0.10, 1.00]$.
3. **Midtone S-Curve Micro-Contrast**: Precomputed 256-entry sigmoid lookup table ($x \rightarrow x + \gamma(x - 0.5)(1 - |2x - 1|)$) eliminates hazy midtones without clipping deep blacks or blown highlights.
4. **Rec. 601 Color Vibrance Lift**: Boosts saturation on muted, washed-out color tones while preserving natural skin tones.

---

## 🏗️ Project Architecture

```
PixLab/
├── include/                     # C++17 Header Files (Interfaces & Types)
│   ├── BMPHandler.h             # 24-bit Windows BMP serializer & deserializer
│   ├── BlurFilter.h             # Separable box blur filter class
│   ├── Filter.h                 # Abstract Filter base class
│   ├── FilterFactory.h          # Polymorphic FilterFactory with parameter scaling
│   ├── GrayscaleFilter.h        # Rec. 601 grayscale filter class
│   ├── Image.h                  # Encapsulated Image class & RGBTRIPLE struct
│   ├── PixelateFilter.h         # Pixelation filter class
│   ├── ReflectFilter.h          # Horizontal reflection filter class
│   ├── SepiaFilter.h            # Sepia matrix filter class
│   ├── UnsharpMaskEnhancer.h    # Multi-stage image clarity & enhancer class
│   └── VignetteFilter.h         # Radial vignette filter class
├── src/                         # C++17 Implementations
│   ├── BMPHandler.cpp
│   ├── BlurFilter.cpp
│   ├── GrayscaleFilter.cpp
│   ├── Image.cpp
│   ├── PixelateFilter.cpp
│   ├── ReflectFilter.cpp
│   ├── SepiaFilter.cpp
│   ├── UnsharpMaskEnhancer.cpp
│   ├── VignetteFilter.cpp
│   └── main.cpp                 # WebAssembly C-linkage export interface
├── tests/
│   └── filter_tests.cpp         # Native C++ unit tests for Image, BMPHandler & Filters
├── images/                      # Website icons, logos, and preview samples
├── index.html                   # Filter Studio with interactive split comparison
├── enhance.html                 # Image Enhancer with percentage strength slider
├── gallery.html                 # PixLab community gallery
├── about.html                   # Project overview & mission
├── script.js                    # WebAssembly memory lifecycle & UI event bridge
├── style.css                    # Responsive dark theme UI styling
├── pixlab_wasm.js               # Dual-mode WebAssembly loader & math engine
├── CMakeLists.txt               # Cross-platform CMake configuration
├── Makefile                     # Command-line & Emscripten build targets
└── package.json                 # Vite development tooling & dependencies
```

---

## ⚡ WebAssembly Memory Architecture

The connection between JavaScript and the C++ WebAssembly engine follows strict memory management practices:

```
[HTML5 Canvas]  ──(imageTo24BitBmp)──> [24-bit BMP in RAM]
                                              │
                                     Module._malloc(size)
                                              │
                                              ▼
                                      [WASM Heap HEAPU8]
                                              │
                                     Module._processImage()
                                              │
                                              ▼
[Multi-Format Download] <──(safe copy)── [Processed BMP]
                                              │
                                     Module._freeBuffer()
```

1. **Zero-Copy Typed Arrays**: Binary BMP buffers are passed across the JS-WASM boundary using raw pointers and typed arrays (`Module.HEAPU8`).
2. **Explicit Deallocation**: Input and output buffers are deallocated via `Module._freeBuffer()` immediately after cloning, resetting the heap allocator to prevent memory leaks.
3. **Resilient Memory Views**: ArrayBuffer detachment checks automatically synchronize memory views if the WebAssembly heap expands.

---

## 🚀 Getting Started

### Prerequisites

- **Node.js** (v18+ recommended)
- **C++17 Compiler** (`g++`, `clang++`, or `MSVC`)
- **Emscripten SDK** (`emcc`) *(optional, required only for recompiling WebAssembly)*

### 1. Clone the Repository
```bash
git clone https://github.com/Sh4uryaa/PixLab.git
cd PixLab
```

### 2. Install Frontend Dependencies & Run Dev Server
```bash
npm install
npm run dev
```
Open `http://localhost:5173` in your browser.

### 3. Build & Run Native C++ Tests
You can compile and run the native C++ test suite using `make` or `cmake`:

**Using Make:**
```bash
make test
```

**Using CMake:**
```bash
mkdir build && cd build
cmake ..
cmake --build .
ctest --output-on-failure
```

### 4. Compile WebAssembly via Emscripten
To recompile the C++ engine to WebAssembly:
```bash
make wasm
```
This generates `pixlab_wasm.js` and `pixlab_wasm.wasm` with optimized `-O3` performance.

---
## 📄 License

This project is licensed under the [MIT License](LICENSE).
