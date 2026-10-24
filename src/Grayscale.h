#ifndef PIXLAB_GRAYSCALE_H
#define PIXLAB_GRAYSCALE_H
#include "Filter.h"

class Grayscale final : public Filter {
public:
    void apply(Image& image) const override;
};
#endif
