#include "UnsharpMaskEnhancer.h"
#include "BlurFilter.h"
#include <cmath>

UnsharpMaskEnhancer::UnsharpMaskEnhancer(float alpha)
    : m_alpha(alpha > 0.0f ? alpha : 1.5f) {}

void UnsharpMaskEnhancer::apply(Image& img) {
    const int width = img.getWidth();
    const int height = img.getHeight();

    // Step 1: Create a deep copy of original image
    Image blurred = img;

    // Step 2: Convolve the copy with 3x3 Box Blur
    BlurFilter blurFilter;
    blurFilter.apply(blurred);

    // Step 3: High-pass difference amplification: orig + (orig - blur) * alpha
    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            RGBTRIPLE& target = img.at(row, col);
            const RGBTRIPLE& blur = blurred.at(row, col);

            int diffR = static_cast<int>(target.r) - static_cast<int>(blur.r);
            int diffG = static_cast<int>(target.g) - static_cast<int>(blur.g);
            int diffB = static_cast<int>(target.b) - static_cast<int>(blur.b);

            int newR = clamp(static_cast<int>(std::round(target.r + diffR * m_alpha)));
            int newG = clamp(static_cast<int>(std::round(target.g + diffG * m_alpha)));
            int newB = clamp(static_cast<int>(std::round(target.b + diffB * m_alpha)));

            target.r = static_cast<uint8_t>(newR);
            target.g = static_cast<uint8_t>(newG);
            target.b = static_cast<uint8_t>(newB);
        }
    }
}
