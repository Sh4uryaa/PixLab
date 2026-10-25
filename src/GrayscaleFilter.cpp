#include "GrayscaleFilter.h"
#include <cmath>

void GrayscaleFilter::apply(Image& img) {
    const int width = img.getWidth();
    const int height = img.getHeight();

    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            RGBTRIPLE& p = img.at(row, col);
            int gray = clamp(static_cast<int>(std::round(
                0.299 * p.r + 0.587 * p.g + 0.114 * p.b
            )));
            p.r = static_cast<uint8_t>(gray);
            p.g = static_cast<uint8_t>(gray);
            p.b = static_cast<uint8_t>(gray);
        }
    }
}
