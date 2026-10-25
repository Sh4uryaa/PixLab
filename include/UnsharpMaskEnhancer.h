#ifndef PIXLAB_UNSHARP_MASK_ENHANCER_H
#define PIXLAB_UNSHARP_MASK_ENHANCER_H

#include "Filter.h"

class UnsharpMaskEnhancer : public Filter {
public:
    explicit UnsharpMaskEnhancer(float alpha = 1.5f);
    void apply(Image& img) override;

    float getAlpha() const noexcept { return m_alpha; }
    void setAlpha(float alpha) noexcept { m_alpha = alpha; }

private:
    float m_alpha;
};

#endif // PIXLAB_UNSHARP_MASK_ENHANCER_H
