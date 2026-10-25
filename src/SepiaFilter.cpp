#include "SepiaFilter.h"
#include <cmath>

void SepiaFilter::apply(Image& img) {
    const int width = img.getWidth();
    const int height = img.getHeight();

    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            RGBTRIPLE& p = img.at(row, col);
            int originalR = p.r;
            int originalG = p.g;
            int originalB = p.b;

            int sepiaR = clamp(static_cast<int>(std::round(
                0.393 * originalR + 0.769 * originalG + 0.189 * originalB
            )));
            int sepiaG = clamp(static_cast<int>(std::round(
                0.349 * originalR + 0.686 * originalG + 0.168 * originalB
            )));
            int sepiaB = clamp(static_cast<int>(std::round(
                0.272 * originalR + 0.534 * originalG + 0.131 * originalB
            )));

            p.r = static_cast<uint8_t>(sepiaR);
            p.g = static_cast<uint8_t>(sepiaG);
            p.b = static_cast<uint8_t>(sepiaB);
        }
    }
}
