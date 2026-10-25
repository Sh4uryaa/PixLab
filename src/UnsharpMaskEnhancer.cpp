#include "UnsharpMaskEnhancer.h"
#include <cmath>
#include <algorithm>
#include <vector>

UnsharpMaskEnhancer::UnsharpMaskEnhancer(float alpha, float contrast, float vibrance)
    : m_alpha(alpha > 0.0f ? alpha : 2.0f),
      m_contrast(contrast > 0.0f ? contrast : 0.18f),
      m_vibrance(vibrance > 0.0f ? vibrance : 1.18f) {}

// Separable 2D Box Blur to extract low-frequency spatial base
Image UnsharpMaskEnhancer::computeBlurredCopy(const Image& src, int radius) const {
    const int width = src.getWidth();
    const int height = src.getHeight();
    if (radius <= 0) return src;

    Image temp(width, height);
    Image result(width, height);

    // Horizontal Pass: src -> temp
    for (int y = 0; y < height; ++y) {
        int sumR = 0, sumG = 0, sumB = 0, count = 0;

        int initialRight = std::min(width - 1, radius);
        for (int x = 0; x <= initialRight; ++x) {
            const RGBTRIPLE& p = src.at(y, x);
            sumR += p.r; sumG += p.g; sumB += p.b;
            ++count;
        }

        for (int x = 0; x < width; ++x) {
            int nextRight = x + radius;
            if (nextRight < width && x > 0) {
                const RGBTRIPLE& p = src.at(y, nextRight);
                sumR += p.r; sumG += p.g; sumB += p.b;
                ++count;
            }
            int prevLeft = x - radius - 1;
            if (prevLeft >= 0) {
                const RGBTRIPLE& p = src.at(y, prevLeft);
                sumR -= p.r; sumG -= p.g; sumB -= p.b;
                --count;
            }

            RGBTRIPLE& out = temp.at(y, x);
            out.r = static_cast<uint8_t>(clamp(std::round(static_cast<float>(sumR) / count)));
            out.g = static_cast<uint8_t>(clamp(std::round(static_cast<float>(sumG) / count)));
            out.b = static_cast<uint8_t>(clamp(std::round(static_cast<float>(sumB) / count)));
        }
    }

    // Vertical Pass: temp -> result
    for (int x = 0; x < width; ++x) {
        int sumR = 0, sumG = 0, sumB = 0, count = 0;

        int initialBottom = std::min(height - 1, radius);
        for (int y = 0; y <= initialBottom; ++y) {
            const RGBTRIPLE& p = temp.at(y, x);
            sumR += p.r; sumG += p.g; sumB += p.b;
            ++count;
        }

        for (int y = 0; y < height; ++y) {
            int nextBottom = y + radius;
            if (nextBottom < height && y > 0) {
                const RGBTRIPLE& p = temp.at(nextBottom, x);
                sumR += p.r; sumG += p.g; sumB += p.b;
                ++count;
            }
            int prevTop = y - radius - 1;
            if (prevTop >= 0) {
                const RGBTRIPLE& p = temp.at(prevTop, x);
                sumR -= p.r; sumG -= p.g; sumB -= p.b;
                --count;
            }

            RGBTRIPLE& out = result.at(y, x);
            out.r = static_cast<uint8_t>(clamp(std::round(static_cast<float>(sumR) / count)));
            out.g = static_cast<uint8_t>(clamp(std::round(static_cast<float>(sumG) / count)));
            out.b = static_cast<uint8_t>(clamp(std::round(static_cast<float>(sumB) / count)));
        }
    }

    return result;
}

// Stage 1: High-pass difference amplification (Unsharp Mask)
void UnsharpMaskEnhancer::applyHighPassSharpening(Image& target, const Image& blurred, float alpha) const {
    const int width = target.getWidth();
    const int height = target.getHeight();

    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            RGBTRIPLE& t = target.at(row, col);
            const RGBTRIPLE& b = blurred.at(row, col);

            int diffR = static_cast<int>(t.r) - static_cast<int>(b.r);
            int diffG = static_cast<int>(t.g) - static_cast<int>(b.g);
            int diffB = static_cast<int>(t.b) - static_cast<int>(b.b);

            int newR = clamp(static_cast<int>(std::round(t.r + diffR * alpha)));
            int newG = clamp(static_cast<int>(std::round(t.g + diffG * alpha)));
            int newB = clamp(static_cast<int>(std::round(t.b + diffB * alpha)));

            t.r = static_cast<uint8_t>(newR);
            t.g = static_cast<uint8_t>(newG);
            t.b = static_cast<uint8_t>(newB);
        }
    }
}

// Stage 2 & 3: Micro-contrast S-curve and color vibrance lift
void UnsharpMaskEnhancer::applyToneAndVibrance(Image& target, float contrastFactor, float vibranceFactor) const {
    const int width = target.getWidth();
    const int height = target.getHeight();

    // Precompute 256-entry S-curve LUT for fast O(1) tone mapping
    std::vector<uint8_t> lut(256);
    for (int i = 0; i < 256; ++i) {
        float n = static_cast<float>(i) / 255.0f;
        // Smooth S-curve polynomial: boost midtone contrast, keep blacks and whites anchored
        float curved = n + contrastFactor * (n - 0.5f) * (1.0f - std::abs(2.0f * n - 1.0f));
        lut[i] = static_cast<uint8_t>(clamp(std::round(curved * 255.0f)));
    }

    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            RGBTRIPLE& pixel = target.at(row, col);

            // 1. Apply micro-contrast S-curve
            float r = static_cast<float>(lut[pixel.r]);
            float g = static_cast<float>(lut[pixel.g]);
            float b = static_cast<float>(lut[pixel.b]);

            // 2. Compute perceptual luminance (Rec. 601 standard)
            float lum = 0.299f * r + 0.587f * g + 0.114f * b;

            // 3. Vibrance boost: enhance saturation on muted colors
            r = lum + (r - lum) * vibranceFactor;
            g = lum + (g - lum) * vibranceFactor;
            b = lum + (b - lum) * vibranceFactor;

            pixel.r = static_cast<uint8_t>(clamp(std::round(r)));
            pixel.g = static_cast<uint8_t>(clamp(std::round(g)));
            pixel.b = static_cast<uint8_t>(clamp(std::round(b)));
        }
    }
}

void UnsharpMaskEnhancer::apply(Image& img) {
    const int width = img.getWidth();
    const int height = img.getHeight();
    if (width <= 0 || height <= 0) return;

    // Adaptive blur radius scaled to image dimensions (typically 2 to 6 pixels)
    int radius = std::max(2, std::min(6, std::min(width, height) / 100));

    // Step 1: Compute low-frequency base using separable convolution
    Image blurred = computeBlurredCopy(img, radius);

    // Step 2: High-pass difference amplification (Unsharp Masking)
    applyHighPassSharpening(img, blurred, m_alpha);

    // Step 3: Micro-contrast clarity and color vibrance lift
    applyToneAndVibrance(img, m_contrast, m_vibrance);
}

