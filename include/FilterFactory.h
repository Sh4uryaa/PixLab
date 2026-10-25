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
                // If param > 5.0f, interpret as user percentage (10.0f - 100.0f)
                // Otherwise interpret as direct alpha factor
                float alpha, contrast, vibrance;
                if (param > 5.0f) {
                    float p = std::max(0.10f, std::min(1.0f, param / 100.0f));
                    alpha = 0.3f + p * 3.3f;
                    contrast = 0.04f + p * 0.28f;
                    vibrance = 1.02f + p * 0.28f;
                } else if (param > 0.0f) {
                    alpha = param;
                    contrast = 0.18f;
                    vibrance = 1.18f;
                } else {
                    alpha = 2.0f;
                    contrast = 0.18f;
                    vibrance = 1.18f;
                }
                return std::make_unique<UnsharpMaskEnhancer>(alpha, contrast, vibrance);
            }
            default:
                throw std::invalid_argument("Unknown filter type ID: " + std::to_string(filterType));
        }
    }
};

#endif // PIXLAB_FILTER_FACTORY_H
