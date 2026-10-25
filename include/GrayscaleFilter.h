#ifndef PIXLAB_GRAYSCALE_FILTER_H
#define PIXLAB_GRAYSCALE_FILTER_H

#include "Filter.h"

class GrayscaleFilter : public Filter {
public:
    void apply(Image& img) override;
};

#endif // PIXLAB_GRAYSCALE_FILTER_H
