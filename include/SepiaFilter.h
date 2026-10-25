#ifndef PIXLAB_SEPIA_FILTER_H
#define PIXLAB_SEPIA_FILTER_H

#include "Filter.h"

class SepiaFilter : public Filter {
public:
    void apply(Image& img) override;
};

#endif // PIXLAB_SEPIA_FILTER_H
