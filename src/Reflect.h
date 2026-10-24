#ifndef PIXLAB_REFLECT_H
#define PIXLAB_REFLECT_H
#include "Filter.h"

class Reflect final : public Filter {
public:
    void apply(Image& image) const override;
};
#endif
