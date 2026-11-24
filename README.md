# PixLab

[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=c%2B%2B)](https://en.cppreference.com/w/cpp/17)
[![WebAssembly](https://img.shields.io/badge/WebAssembly-WASM-654FF0?logo=webassembly)](https://webassembly.org/)
[![Vite](https://img.shields.io/badge/Vite-8.3-646CFF?logo=vite)](https://vitejs.dev/)
[![Firebase](https://img.shields.io/badge/Firebase-Auth%20%26%20Hosting-FFCA28?logo=firebase)](https://firebase.google.com/)

> **A high-performance, client-side image processing and photo enhancement application powered by an object-oriented C++17 engine compiled to WebAssembly (WASM).**

🌐 **Live Application:** [https://pixlab-a564f.web.app/](https://pixlab-a564f.web.app/)

---

## Overview

PixLab delivers desktop-class image processing directly in the web browser. By executing pure arithmetic C++ algorithms inside a sandboxed WebAssembly runtime, PixLab processes images locally with zero cloud latency, zero bandwidth overhead, and complete user privacy.

The architecture combines clean object-oriented design patterns with low-level algorithmic optimizations (fixed-point arithmetic, cache-friendly row pointers, and bulk memory transfers) to process high-resolution photos in milliseconds.

---

## Key Features

* **Real-Time Interactive Comparison:** Smooth before-and-after split slider with automatic natural aspect-ratio preservation.
* **Universal Format Ingestion:** Seamless drag-and-drop and upload support for PNG, JPG/JPEG, BMP, and WebP.
* **Multi-Format Export Engine:** High-speed client-side export in **PNG**, **JPG**, or **BMP** with solid background alpha flattening for JPEGs.
* **Multi-Stage Clarity & Enhancement Engine:** A 3-stage photography enhancement pipeline featuring adaptive separable low-frequency extraction, high-pass unsharp masking, midtone S-curve micro-contrast mapping, and Rec. 601 perceptual luminance vibrance lifting.
* **Classic & Artistic Filter Suite:** Real-time Grayscale, Sepia, Multi-pass Blur, Horizontal Reflection, Pixelation, and Radial Vignette.
* **Cloud Authentication & Gallery:** Firebase Auth integration supporting Email/Password and one-click Google OAuth with secure user state persistence.
* **Automated Test Suite:** Built-in Node.js and C++ regression tests verifying header serialization, memory cleanup, and algorithmic accuracy.

---

## Architectural Highlights & Engineering

```
+-----------------------------------------------------------------------------------+
|                               PIXLAB ARCHITECTURE                                 |
+-----------------------------------------------------------------------------------+
|  Browser UI (HTML5 / CSS3 / ES Modules / Canvas 2D)                               |
|       |                                                                           |
|       v  (32-bit packed RGBA unpack / pack via Uint32Array)                       |
|  JavaScript Bridge (script.js)                                                    |
|       |                                                                           |
|       v  (Linear heap allocation: _malloc / _free / HEAPU8)                       |
|  WebAssembly Runtime (pixlab_wasm.js + pixlab_wasm.wasm)                          |
|       |                                                                           |
|       v  (Polymorphic FilterFactory dynamic dispatch)                             |
|  C++17 Engine Core                                                                |
|   +-----------------------------------------------------------------------------+ |
|   | Encapsulated Image Model: Packed RGBTRIPLE scanlines & inlined row() pointers| |
|   | BMPHandler: Bulk std::memcpy scanline serialization with overflow guards   | |
|   | 16-bit Fixed-Point Integer Math: Zero float/std::round overhead in inner loop | |
|   | Square-Root-Free Vignette: Precomputed quadratic inverse radial falloff     | |
|   | Ping-Pong Separable Blur: Zero heap allocations across convolution passes   | |
|   +-----------------------------------------------------------------------------+ |
+-----------------------------------------------------------------------------------+
```

### 1. Object-Oriented C++17 Core
* **Polymorphism & Abstraction:** An abstract base class `Filter` defines the pure virtual `apply(Image& img)` contract.
* **Factory Pattern:** `FilterFactory::create(int type, float param)` dynamically instantiates filter subclasses via `std::unique_ptr<Filter>`.
* **Encapsulated Buffer:** The `Image` class encapsulates flat contiguous pixel memory (`std::vector<RGBTRIPLE>`), providing both bounds-checked access (`at()`) and high-performance inlined scanline row pointers (`row(r)` and `operator()`).
* **Binary Serialization:** `BMPHandler` deserializes in-memory 24-bit uncompressed BMP scanlines directly into image buffers using bulk `std::memcpy`.

### 2. Algorithmic Optimizations
* **16-bit Fixed-Point Arithmetic:** Grayscale and Sepia matrix multiplications are scaled by $2^{16}$ (65536) with a $2^{15}$ (32768) rounding bias. This eliminates floating-point conversions and `std::round` calls inside hot loops, yielding a 5x–10x speedup and auto-vectorization.
* **Square-Root-Free Vignette:** Vignette falloff replaces expensive $O(W \times H)$ `std::sqrt` calculations by operating directly on squared radial distances $(dx^2 + dy^2) \cdot \text{invMaxDist}^2$.
* **Zero-Allocation Ping-Pong Blur:** Multi-pass blur leverages a single temporary scratch buffer, swapping source and destination pointers across passes to eliminate memory re-allocations.
* **32-Bit Pixel Bridge:** The JavaScript canvas bridge unpacks and packs 32-bit words (`Uint32Array`) directly into `ImageData.data.buffer`, reducing memory bus transactions 4x compared to per-channel loops.

---

## Filter & Algorithm Reference

| ID | Filter Name | Core Formula / Algorithm | Complexity | Parameters |
|---|---|---|---|---|
| **1** | **Grayscale** | $\text{Gray} = (19595 \cdot R + 38470 \cdot G + 7471 \cdot B + 32768) \gg 16$ (Rec. 601) | $O(N)$ | None |
| **2** | **Sepia** | Fixed-point affine color transformation matrix | $O(N)$ | None |
| **3** | **Blur** | Multi-pass convolution with ping-pong buffering | $O(k \cdot N)$ | None |
| **4** | **Reflection** | Horizontal scanline swap: `std::swap(row[c], row[w - 1 - c])` | $O(N / 2)$ | None |
| **5** | **Pixelate** | $B \times B$ spatial block averaging with integer division | $O(N)$ | Block size (default: 10px) |
| **6** | **Vignette** | Quadratic falloff: $1.0 - (dx^2 + dy^2) \cdot \frac{0.75}{\text{maxDist}^2}$ | $O(N)$ | None |
| **7** | **Enhance (Unsharp Mask)** | Adaptive separable spatial blur base + High-pass difference amplification + S-curve midtone contrast LUT + Rec. 601 vibrance lift | $O(N)$ | Intensity % ($10\% - 100\%$) |

---

## Getting Started

### Prerequisites

* **Node.js** (v18.0.0 or later)
* **npm** (v9.0.0 or later)
* *(Optional for native C++ compilation)*: Modern C++17 compiler (`g++` or `clang++`) or [Emscripten SDK](https://emscripten.org/) (`emcc`).

### Installation & Local Development

1. **Clone the repository:**
   ```bash
   git clone https://github.com/Sh4uryaa/PixLab.git
   cd PixLab
   ```

2. **Install frontend dependencies:**
   ```bash
   npm install
   ```

3. **Start the local Vite development server:**
   ```bash
   npm run dev
   ```
   Open your browser at `http://localhost:5173/`.

4. **Execute the automated test suite:**
   ```bash
   npm test
   ```

5. **Build for production:**
   ```bash
   npm run build
   ```
   The compiled, optimized distribution bundles are written to the `dist/` directory.

---

## Compiling the C++ Engine (Optional)

PixLab includes a precompiled WebAssembly runtime in `public/` and `pixlab_wasm.js`. If you wish to recompile the C++ source code manually:

### Native Desktop CLI & Unit Tests (GCC/Clang)
```bash
# Build native desktop executable
make cli

# Run C++ unit test suite
make test
```

### WebAssembly via Emscripten
```bash
# Compile C++17 engine into WebAssembly binary
make wasm
```

Or using **CMake**:
```bash
mkdir build && cd build
cmake ..
cmake --build .
```

---

## Verification & Performance Benchmarks

The automated test harness ([tests/node_filter_tests.js](tests/node_filter_tests.js)) benchmarks filter execution across a standard 24-bit test image:

```text
==================================================
 PixLab Automated Test & Benchmark Suite (Node.js)
==================================================
[Setup] Created 128x128 24-bit BMP test image (49206 bytes)
  ✓ Filter 1 [Grayscale (Rec. 601 16-bit Fixed-Point)]: PASSED (1.90 ms)
  ✓ Filter 2 [Sepia (16-bit Fixed-Point Matrix)]: PASSED (1.60 ms)
  ✓ Filter 3 [Blur (Separable Multi-Pass)]: PASSED (10.87 ms)
  ✓ Filter 4 [Reflection (Horizontal In-Place)]: PASSED (1.21 ms)
  ✓ Filter 5 [Pixelate (10px Block Average)]: PASSED (1.12 ms)
  ✓ Filter 6 [Vignette (Square-Root-Free Falloff)]: PASSED (2.62 ms)
  ✓ Filter 7 [Unsharp Mask & Clarity Enhancer]: PASSED (9.89 ms)
  ✓ Edge case: Empty input buffer rejected safely.
  ✓ Edge case: Corrupt header rejected safely.
==================================================
 Results: 9 / 9 tests passed!
==================================================
```

---

## Planned Enhancements

* [ ] **SIMD Acceleration:** Utilize WebAssembly 128-bit SIMD intrinsics (`wasm_simd128.h`) for 4-wide vector arithmetic.
* [ ] **Web Workers:** Offload image processing to dedicated Background Web Workers to eliminate any main-thread UI blocking on ultra-large (30MP+) RAW images.
* [ ] **Extended Filter Library:** Add Sobel edge detection, Laplacian high-pass, Bilateral edge-preserving smoothing, and custom convolution matrix inputs.
* [ ] **Cloud Gallery Sync:** Direct Cloud Firestore integration for user gallery persistence.
