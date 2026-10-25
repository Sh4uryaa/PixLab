#include "PixelateFilter.h"
#include <algorithm>
#include <cmath>

PixelateFilter::PixelateFilter(int blockSize)
    : m_blockSize(blockSize > 0 ? blockSize : 10) {}

void PixelateFilter::apply(Image& img) {
    const int width = img.getWidth();
    const int height = img.getHeight();

    for (int r0 = 0; r0 < height; r0 += m_blockSize) {
        int rEnd = std::min(r0 + m_blockSize, height);

        for (int c0 = 0; c0 < width; c0 += m_blockSize) {
            int cEnd = std::min(c0 + m_blockSize, width);

            int sumR = 0;
            int sumG = 0;
            int sumB = 0;
            int count = 0;

            for (int r = r0; r < rEnd; ++r) {
                for (int c = c0; c < cEnd; ++c) {
                    const RGBTRIPLE& p = img.at(r, c);
                    sumR += p.r;
                    sumG += p.g;
                    sumB += p.b;
                    ++count;
                }
            }

            if (count > 0) {
                uint8_t avgR = clamp(static_cast<int>(std::round(static_cast<double>(sumR) / count)));
                uint8_t avgG = clamp(static_cast<int>(std::round(static_cast<double>(sumG) / count)));
                uint8_t avgB = clamp(static_cast<int>(std::round(static_cast<double>(sumB) / count)));

                for (int r = r0; r < rEnd; ++r) {
                    for (int c = c0; c < cEnd; ++c) {
                        RGBTRIPLE& p = img.at(r, c);
                        p.r = avgR;
                        p.g = avgG;
                        p.b = avgB;
                    }
                }
            }
        }
    }
}
