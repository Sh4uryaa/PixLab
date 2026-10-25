#ifndef PIXLAB_BLUR_FILTER_H
#define PIXLAB_BLUR_FILTER_H

#include "Filter.h"

class BlurFilter : public Filter {
public:
    void apply(Image& img) override;
};

#endif // PIXLAB_BLUR_FILTER_H
