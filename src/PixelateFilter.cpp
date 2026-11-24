#include "PixelateFilter.h"
#include <algorithm>

PixelateFilter::PixelateFilter(int blockSize)
    : m_blockSize(blockSize > 0 ? blockSize : 10) {}

void PixelateFilter::apply(Image& img) {
    const int width = img.getWidth();
    const int height = img.getHeight();
    const int blockSize = m_blockSize > 0 ? m_blockSize : 10;

    for (int r0 = 0; r0 < height; r0 += blockSize) {
        const int rEnd = std::min(r0 + blockSize, height);

        for (int c0 = 0; c0 < width; c0 += blockSize) {
            const int cEnd = std::min(c0 + blockSize, width);

            int sumR = 0;
            int sumG = 0;
            int sumB = 0;
            const int count = (rEnd - r0) * (cEnd - c0);
            if (count <= 0) continue;

            for (int r = r0; r < rEnd; ++r) {
                const RGBTRIPLE* rPtr = img.row(r);
                for (int c = c0; c < cEnd; ++c) {
                    sumR += rPtr[c].r;
                    sumG += rPtr[c].g;
                    sumB += rPtr[c].b;
                }
            }

            const int halfCount = count >> 1;
            const RGBTRIPLE avgPixel{
                static_cast<uint8_t>((sumB + halfCount) / count),
                static_cast<uint8_t>((sumG + halfCount) / count),
                static_cast<uint8_t>((sumR + halfCount) / count)
            };

            for (int r = r0; r < rEnd; ++r) {
                RGBTRIPLE* rPtr = img.row(r);
                for (int c = c0; c < cEnd; ++c) {
                    rPtr[c] = avgPixel;
                }
            }
        }
    }
}
