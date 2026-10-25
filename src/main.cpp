#include "BMPHandler.h"
#include "FilterFactory.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <vector>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#define PIXLAB_EXPORT EMSCRIPTEN_KEEPALIVE
#else
#define PIXLAB_EXPORT
#endif

// Internal state tracking for the last processed output buffer size
static size_t g_lastOutputSize = 0;

extern "C" {

/**
 * Processes an in-memory 24-bit BMP image using pure arithmetic C++ filters.
 *
 * @param inputBuffer Pointer to contiguous BMP bytes in WASM heap
 * @param inputSize   Size of input BMP buffer in bytes
 * @param filterCode  Numeric ID matching FilterFactory::FilterType (1-7)
 * @param param       Optional floating-point parameter (e.g. block size or sharpness alpha)
 * @return Pointer to freshly allocated BMP buffer on WASM heap, or nullptr on failure.
 */
PIXLAB_EXPORT uint8_t* processImage(const uint8_t* inputBuffer, size_t inputSize, int filterCode, float param) {
    g_lastOutputSize = 0;

    if (!inputBuffer || inputSize == 0) {
        std::cerr << "[PixLab WASM] Error: Empty input buffer.\n";
        return nullptr;
    }

    try {
        // Step 1: Deserialize BMP binary from RAM into Encapsulated Image
        Image img = BMPHandler::deserialize(inputBuffer, inputSize);

        // Step 2: Use Polymorphic Factory to instantiate the requested Filter subclass
        std::unique_ptr<Filter> filter = FilterFactory::create(filterCode, param);

        // Step 3: Apply filter transformation in-place
        filter->apply(img);

        // Step 4: Serialize transformed Image back to 24-bit BMP format
        std::vector<uint8_t> outputData = BMPHandler::serialize(img);

        g_lastOutputSize = outputData.size();

        // Step 5: Allocate memory on heap for JavaScript to read
        uint8_t* outPtr = static_cast<uint8_t*>(std::malloc(g_lastOutputSize));
        if (!outPtr) {
            std::cerr << "[PixLab WASM] Error: Memory allocation failed for output.\n";
            g_lastOutputSize = 0;
            return nullptr;
        }

        std::memcpy(outPtr, outputData.data(), g_lastOutputSize);
        return outPtr;

    } catch (const std::exception& ex) {
        std::cerr << "[PixLab WASM] Exception during image processing: " << ex.what() << "\n";
        g_lastOutputSize = 0;
        return nullptr;
    } catch (...) {
        std::cerr << "[PixLab WASM] Unknown exception occurred during image processing.\n";
        g_lastOutputSize = 0;
        return nullptr;
    }
}

/**
 * Returns the byte size of the most recently produced output BMP buffer.
 */
PIXLAB_EXPORT size_t getOutputSize() {
    return g_lastOutputSize;
}

/**
 * Safely frees heap memory allocated either by malloc in JS or processImage in WASM.
 */
PIXLAB_EXPORT void freeBuffer(uint8_t* ptr) {
    if (ptr) {
        std::free(ptr);
    }
}

} // extern "C"

#if !defined(__EMSCRIPTEN__)
// CLI entry point for native desktop compilation and testing
int main(int argc, char* argv[]) {
    std::cout << "PixLab C++17 Core Image Processing Engine (Academic Edition)\n";
    std::cout << "Target: WebAssembly (WASM) via Emscripten.\n";
    return 0;
}
#endif
