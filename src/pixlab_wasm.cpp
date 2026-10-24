// Optional Emscripten/WebAssembly interface for browser integration.
// Input and output buffers use RGBA byte order (R,G,B,A), unlike BMP's BGR storage.
#include "Blur.h"
#include "Grayscale.h"
#include "Reflect.h"
#include "Sepia.h"
#include "Image.h"

#include <algorithm>
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#define PIXLAB_EXPORT EMSCRIPTEN_KEEPALIVE
#else
#define PIXLAB_EXPORT
#endif

extern "C" {
PIXLAB_EXPORT int pixlab_apply_filter(std::uint8_t* rgba, int width, int height, int filterId) {
    if (!rgba || width <= 0 || height <= 0 ||
        static_cast<std::uint64_t>(width) * height > 100000000ULL ||
        filterId < 0 || filterId > 3) return 0;

    // Build a lightweight in-memory Image using a generated 24-bit BMP buffer would
    // be wasteful. Keep this adapter independent by implementing the same algorithms
    // on RGBA pixels directly, preserving alpha.
    const std::size_t count = static_cast<std::size_t>(width) * height;
    std::vector<std::uint8_t> source(rgba, rgba + count * 4);
    auto clampRound = [](double value) -> std::uint8_t {
        return static_cast<std::uint8_t>(std::clamp(static_cast<int>(value + 0.5), 0, 255));
    };

    if (filterId == 0) { // grayscale
        for (std::size_t i = 0; i < count; ++i) {
            const int avg = static_cast<int>(std::round(
                (source[i*4] + source[i*4+1] + source[i*4+2]) / 3.0));
            rgba[i*4] = rgba[i*4+1] = rgba[i*4+2] = static_cast<std::uint8_t>(avg);
        }
    } else if (filterId == 1) { // sepia
        for (std::size_t i = 0; i < count; ++i) {
            const int r = source[i*4], g = source[i*4+1], b = source[i*4+2];
            rgba[i*4] = clampRound(.393*r + .769*g + .189*b);
            rgba[i*4+1] = clampRound(.349*r + .686*g + .168*b);
            rgba[i*4+2] = clampRound(.272*r + .534*g + .131*b);
        }
    } else if (filterId == 2) { // reflect
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const std::size_t from = (static_cast<std::size_t>(y)*width + (width-1-x))*4;
                const std::size_t to = (static_cast<std::size_t>(y)*width + x)*4;
                for (int c = 0; c < 4; ++c) rgba[to+c] = source[from+c];
            }
        }
    } else { // blur
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int sums[3] = {0,0,0}, neighbors = 0;
                for (int yy = y-1; yy <= y+1; ++yy) {
                    for (int xx = x-1; xx <= x+1; ++xx) {
                        if (yy < 0 || yy >= height || xx < 0 || xx >= width) continue;
                        const std::size_t pos = (static_cast<std::size_t>(yy)*width + xx)*4;
                        for (int c = 0; c < 3; ++c) sums[c] += source[pos+c];
                        ++neighbors;
                    }
                }
                const std::size_t out = (static_cast<std::size_t>(y)*width + x)*4;
                for (int c = 0; c < 3; ++c)
                    rgba[out+c] = static_cast<std::uint8_t>(std::round(sums[c] / static_cast<double>(neighbors)));
            }
        }
    }
    return 1;
}
}
