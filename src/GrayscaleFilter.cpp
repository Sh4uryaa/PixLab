#include "GrayscaleFilter.h"

void GrayscaleFilter::apply(Image& img) {
    RGBTRIPLE* data = img.data();
    const std::size_t totalPixels = img.getPixelCount();

    // High-performance Rec. 601 luminance using 16-bit fixed-point arithmetic:
    // 0.299 * 65536 = 19595, 0.587 * 65536 = 38470, 0.114 * 65536 = 7471
    // Sum = 65536 (2^16). Rounding bias = 32768 (2^15).
    // Vectorizable linear loop with zero floating-point conversions or function calls.
    for (std::size_t i = 0; i < totalPixels; ++i) {
        const int r = data[i].r;
        const int g = data[i].g;
        const int b = data[i].b;
        const uint8_t gray = static_cast<uint8_t>((19595 * r + 38470 * g + 7471 * b + 32768) >> 16);
        data[i].r = gray;
        data[i].g = gray;
        data[i].b = gray;
    }
}
