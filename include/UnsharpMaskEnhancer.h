#ifndef PIXLAB_UNSHARP_MASK_ENHANCER_H
#define PIXLAB_UNSHARP_MASK_ENHANCER_H

#include "Filter.h"

/**
 * @brief Professional Multi-Stage Image Clarity & Enhancement Engine
 *
 * Implements an advanced, purely mathematical image enhancement pipeline (ZERO ML/AI):
 * 1. Adaptive Multi-Scale Spatial Blur convolution to isolate high-frequency edge textures
 * 2. Unsharp Masking (High-Pass Detail Amplification) with configurable alpha strength
 * 3. Micro-Contrast & Dynamic Range Expansion via midtone S-curve sigmoid tone mapping
 * 4. Color Vibrance & Luminance-Aware Saturation Lift to restore dull, washed-out tones
 *
 * Strictly adheres to C++ OOP Encapsulation and Polymorphism inheriting from Filter.
 */
class UnsharpMaskEnhancer : public Filter {
public:
    explicit UnsharpMaskEnhancer(float alpha = 2.0f, float contrast = 0.18f, float vibrance = 1.18f);
    void apply(Image& img) override;

    float getAlpha() const noexcept { return m_alpha; }
    void setAlpha(float alpha) noexcept { m_alpha = alpha; }

    float getContrast() const noexcept { return m_contrast; }
    void setContrast(float contrast) noexcept { m_contrast = contrast; }

    float getVibrance() const noexcept { return m_vibrance; }
    void setVibrance(float vibrance) noexcept { m_vibrance = vibrance; }

private:
    float m_alpha;     // Edge sharpness amplification factor
    float m_contrast;  // S-curve micro-contrast boost factor
    float m_vibrance;  // Color saturation lift factor

    // Encapsulated mathematical subroutines
    Image computeBlurredCopy(const Image& src, int radius) const;
    void applyHighPassSharpening(Image& target, const Image& blurred, float alpha) const;
    void applyToneAndVibrance(Image& target, float contrastFactor, float vibranceFactor) const;
};

#endif // PIXLAB_UNSHARP_MASK_ENHANCER_H

