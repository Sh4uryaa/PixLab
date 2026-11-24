#include "UnsharpMaskEnhancer.h"
#include <array>
#include <cmath>
#include <algorithm>

UnsharpMaskEnhancer::UnsharpMaskEnhancer(float alpha, float contrast, float vibrance)
    : m_alpha(alpha > 0.0f ? alpha : 2.0f),
      m_contrast(contrast > 0.0f ? contrast : 0.18f),
      m_vibrance(vibrance > 0.0f ? vibrance : 1.18f) {}

// Separable 2D Box Blur with sliding window to extract low-frequency spatial base
Image UnsharpMaskEnhancer::computeBlurredCopy(const Image& src, int radius) const {
    const int width = src.getWidth();
    const int height = src.getHeight();
    if (radius <= 0) return src;

    Image temp(width, height);
    Image result(width, height);

    // Horizontal Pass: src -> temp using direct scanline pointers
    for (int y = 0; y < height; ++y) {
        const RGBTRIPLE* srcRow = src.row(y);
        RGBTRIPLE* tempRow = temp.row(y);

        int sumR = 0, sumG = 0, sumB = 0, count = 0;

        int initialRight = std::min(width - 1, radius);
        for (int x = 0; x <= initialRight; ++x) {
            const RGBTRIPLE& p = srcRow[x];
            sumR += p.r; sumG += p.g; sumB += p.b;
            ++count;
        }

        for (int x = 0; x < width; ++x) {
            int nextRight = x + radius;
            if (nextRight < width && x > 0) {
                const RGBTRIPLE& p = srcRow[nextRight];
                sumR += p.r; sumG += p.g; sumB += p.b;
                ++count;
            }
            int prevLeft = x - radius - 1;
            if (prevLeft >= 0) {
                const RGBTRIPLE& p = srcRow[prevLeft];
                sumR -= p.r; sumG -= p.g; sumB -= p.b;
                --count;
            }

            const int half = count >> 1;
            tempRow[x].r = static_cast<uint8_t>((sumR + half) / count);
            tempRow[x].g = static_cast<uint8_t>((sumG + half) / count);
            tempRow[x].b = static_cast<uint8_t>((sumB + half) / count);
        }
    }

    // Vertical Pass: temp -> result
    for (int x = 0; x < width; ++x) {
        int sumR = 0, sumG = 0, sumB = 0, count = 0;

        int initialBottom = std::min(height - 1, radius);
        for (int y = 0; y <= initialBottom; ++y) {
            const RGBTRIPLE& p = temp(y, x);
            sumR += p.r; sumG += p.g; sumB += p.b;
            ++count;
        }

        for (int y = 0; y < height; ++y) {
            int nextBottom = y + radius;
            if (nextBottom < height && y > 0) {
                const RGBTRIPLE& p = temp(nextBottom, x);
                sumR += p.r; sumG += p.g; sumB += p.b;
                ++count;
            }
            int prevTop = y - radius - 1;
            if (prevTop >= 0) {
                const RGBTRIPLE& p = temp(prevTop, x);
                sumR -= p.r; sumG -= p.g; sumB -= p.b;
                --count;
            }

            const int half = count >> 1;
            result(y, x).r = static_cast<uint8_t>((sumR + half) / count);
            result(y, x).g = static_cast<uint8_t>((sumG + half) / count);
            result(y, x).b = static_cast<uint8_t>((sumB + half) / count);
        }
    }

    return result;
}

// Stage 1: High-pass difference amplification (Unsharp Mask) with flat contiguous iteration
void UnsharpMaskEnhancer::applyHighPassSharpening(Image& target, const Image& blurred, float alpha) const {
    RGBTRIPLE* t = target.data();
    const RGBTRIPLE* b = blurred.data();
    const std::size_t totalPixels = target.getPixelCount();

    for (std::size_t i = 0; i < totalPixels; ++i) {
        const int diffR = static_cast<int>(t[i].r) - static_cast<int>(b[i].r);
        const int diffG = static_cast<int>(t[i].g) - static_cast<int>(b[i].g);
        const int diffB = static_cast<int>(t[i].b) - static_cast<int>(b[i].b);

        t[i].r = clamp(static_cast<int>(t[i].r + diffR * alpha + 0.5f));
        t[i].g = clamp(static_cast<int>(t[i].g + diffG * alpha + 0.5f));
        t[i].b = clamp(static_cast<int>(t[i].b + diffB * alpha + 0.5f));
    }
}

// Stage 2 & 3: Micro-contrast S-curve and color vibrance lift
void UnsharpMaskEnhancer::applyToneAndVibrance(Image& target, float contrastFactor, float vibranceFactor) const {
    // Precompute 256-entry S-curve LUT on stack (zero heap allocation)
    std::array<uint8_t, 256> lut;
    for (int i = 0; i < 256; ++i) {
        float n = static_cast<float>(i) / 255.0f;
        // Smooth S-curve polynomial: boost midtone contrast, keep blacks and whites anchored
        float curved = n + contrastFactor * (n - 0.5f) * (1.0f - std::abs(2.0f * n - 1.0f));
        lut[i] = static_cast<uint8_t>(clamp(static_cast<int>(curved * 255.0f + 0.5f)));
    }

    RGBTRIPLE* data = target.data();
    const std::size_t totalPixels = target.getPixelCount();

    for (std::size_t i = 0; i < totalPixels; ++i) {
        // 1. Apply micro-contrast S-curve
        float r = static_cast<float>(lut[data[i].r]);
        float g = static_cast<float>(lut[data[i].g]);
        float b = static_cast<float>(lut[data[i].b]);

        // 2. Compute perceptual luminance (Rec. 601 standard)
        float lum = 0.299f * r + 0.587f * g + 0.114f * b;

        // 3. Vibrance boost: enhance saturation on muted colors
        r = lum + (r - lum) * vibranceFactor;
        g = lum + (g - lum) * vibranceFactor;
        b = lum + (b - lum) * vibranceFactor;

        data[i].r = static_cast<uint8_t>(clamp(static_cast<int>(r + 0.5f)));
        data[i].g = static_cast<uint8_t>(clamp(static_cast<int>(g + 0.5f)));
        data[i].b = static_cast<uint8_t>(clamp(static_cast<int>(b + 0.5f)));
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

