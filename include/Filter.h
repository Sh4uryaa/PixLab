#ifndef PIXLAB_FILTER_H
#define PIXLAB_FILTER_H

#include "Image.h"
#include <cstdint>

class Filter {
public:
    virtual ~Filter() = default;

    // Pure virtual method demonstrating polymorphism and abstraction
    virtual void apply(Image& img) = 0;

protected:
    // Protected pixel clamping utility to ensure RGB bounds [0, 255]
    static uint8_t clamp(int val) noexcept {
        return val < 0 ? 0 : (val > 255 ? 255 : static_cast<uint8_t>(val));
    }
};

#endif // PIXLAB_FILTER_H
