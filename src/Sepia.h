#ifndef PIXLAB_SEPIA_H
#define PIXLAB_SEPIA_H
#include "Filter.h"

class Sepia final : public Filter {
public:
    void apply(Image& image) const override;
};
#endif
