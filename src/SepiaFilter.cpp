#include "SepiaFilter.h"

void SepiaFilter::apply(Image& img) {
    RGBTRIPLE* data = img.data();
    const std::size_t totalPixels = img.getPixelCount();

    // High-performance Sepia transformation using 16-bit fixed-point arithmetic:
    // Weights scaled by 65536 (2^16) with rounding bias 32768 (2^15):
    // R: 0.393 -> 25756, 0.769 -> 50397, 0.189 -> 12386
    // G: 0.349 -> 22872, 0.686 -> 44958, 0.168 -> 11010
    // B: 0.272 -> 17826, 0.534 -> 34996, 0.131 -> 8585
    for (std::size_t i = 0; i < totalPixels; ++i) {
        const int originalR = data[i].r;
        const int originalG = data[i].g;
        const int originalB = data[i].b;

        const int sepiaR = (25756 * originalR + 50397 * originalG + 12386 * originalB + 32768) >> 16;
        const int sepiaG = (22872 * originalR + 44958 * originalG + 11010 * originalB + 32768) >> 16;
        const int sepiaB = (17826 * originalR + 34996 * originalG + 8585 * originalB + 32768) >> 16;

        data[i].r = clamp(sepiaR);
        data[i].g = clamp(sepiaG);
        data[i].b = clamp(sepiaB);
    }
}
