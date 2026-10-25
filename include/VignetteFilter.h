#ifndef PIXLAB_VIGNETTE_FILTER_H
#define PIXLAB_VIGNETTE_FILTER_H

#include "Filter.h"

class VignetteFilter : public Filter {
public:
    void apply(Image& img) override;
};

#endif // PIXLAB_VIGNETTE_FILTER_H
