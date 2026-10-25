#include "BlurFilter.h"
#include <cmath>

void BlurFilter::apply(Image& img) {
    const int width = img.getWidth();
    const int height = img.getHeight();

    // 4 passes of 3x3 convolution to create a clearly visible, smooth Gaussian-like blur
    for (int pass = 0; pass < 4; ++pass) {
        Image reference = img;
        for (int row = 0; row < height; ++row) {
            for (int col = 0; col < width; ++col) {
                int sumR = 0;
                int sumG = 0;
                int sumB = 0;
                int count = 0;

                for (int dr = -1; dr <= 1; ++dr) {
                    int nr = row + dr;
                    if (nr < 0 || nr >= height) continue;

                    for (int dc = -1; dc <= 1; ++dc) {
                        int nc = col + dc;
                        if (nc < 0 || nc >= width) continue;

                        const RGBTRIPLE& neighbor = reference.at(nr, nc);
                        sumR += neighbor.r;
                        sumG += neighbor.g;
                        sumB += neighbor.b;
                        ++count;
                    }
                }

                RGBTRIPLE& target = img.at(row, col);
                target.r = clamp(static_cast<int>(std::round(static_cast<double>(sumR) / count)));
                target.g = clamp(static_cast<int>(std::round(static_cast<double>(sumG) / count)));
                target.b = clamp(static_cast<int>(std::round(static_cast<double>(sumB) / count)));
            }
        }
    }
}
