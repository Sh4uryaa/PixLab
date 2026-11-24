#include "BlurFilter.h"
#include <algorithm>

void BlurFilter::apply(Image& img) {
    const int width = img.getWidth();
    const int height = img.getHeight();
    if (width <= 0 || height <= 0) return;

    // Allocate a single scratch buffer for ping-pong processing across 4 passes
    Image temp(width, height);
    Image* src = &img;
    Image* dst = &temp;

    for (int pass = 0; pass < 4; ++pass) {
        for (int row = 0; row < height; ++row) {
            const int rMin = std::max(0, row - 1);
            const int rMax = std::min(height - 1, row + 1);
            RGBTRIPLE* dstRow = dst->row(row);

            for (int col = 0; col < width; ++col) {
                const int cMin = std::max(0, col - 1);
                const int cMax = std::min(width - 1, col + 1);

                int sumR = 0;
                int sumG = 0;
                int sumB = 0;
                int count = 0;

                for (int nr = rMin; nr <= rMax; ++nr) {
                    const RGBTRIPLE* srcRow = src->row(nr);
                    for (int nc = cMin; nc <= cMax; ++nc) {
                        sumR += srcRow[nc].r;
                        sumG += srcRow[nc].g;
                        sumB += srcRow[nc].b;
                        ++count;
                    }
                }

                // Integer rounding eliminates double conversions and std::round overhead
                const int halfCount = count >> 1;
                dstRow[col].r = static_cast<uint8_t>((sumR + halfCount) / count);
                dstRow[col].g = static_cast<uint8_t>((sumG + halfCount) / count);
                dstRow[col].b = static_cast<uint8_t>((sumB + halfCount) / count);
            }
        }
        std::swap(src, dst);
    }
}
