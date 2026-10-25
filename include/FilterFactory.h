#ifndef PIXLAB_FILTER_FACTORY_H
#define PIXLAB_FILTER_FACTORY_H

#include "Filter.h"
#include "GrayscaleFilter.h"
#include "SepiaFilter.h"
#include "BlurFilter.h"
#include "ReflectFilter.h"
#include "PixelateFilter.h"
#include "VignetteFilter.h"
#include "UnsharpMaskEnhancer.h"

#include <memory>
#include <stdexcept>

class FilterFactory {
public:
    enum FilterType {
        GRAYSCALE = 1,
        SEPIA = 2,
        BLUR = 3,
        REFLECT = 4,
        PIXELATE = 5,
        VIGNETTE = 6,
        ENHANCE_UNSHARP = 7
    };

    // Polymorphic factory method creating filters based on ID and parameters
    static std::unique_ptr<Filter> create(int filterType, float param = 0.0f) {
        switch (filterType) {
            case GRAYSCALE:
                return std::make_unique<GrayscaleFilter>();
            case SEPIA:
                return std::make_unique<SepiaFilter>();
            case BLUR:
                return std::make_unique<BlurFilter>();
            case REFLECT:
                return std::make_unique<ReflectFilter>();
            case PIXELATE: {
                int blockSize = param > 0.0f ? static_cast<int>(param) : 10;
                return std::make_unique<PixelateFilter>(blockSize);
            }
            case VIGNETTE:
                return std::make_unique<VignetteFilter>();
            case ENHANCE_UNSHARP: {
                float alpha = param > 0.0f ? param : 1.5f;
                return std::make_unique<UnsharpMaskEnhancer>(alpha);
            }
            default:
                throw std::invalid_argument("Unknown filter type ID: " + std::to_string(filterType));
        }
    }
};

#endif // PIXLAB_FILTER_FACTORY_H
