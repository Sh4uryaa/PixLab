#ifndef PIXLAB_BLUR_H
#define PIXLAB_BLUR_H
#include "Filter.h"

class Blur final : public Filter {
public:
    void apply(Image& image) const override;
};
#endif
