#ifndef PIXLAB_REFLECT_FILTER_H
#define PIXLAB_REFLECT_FILTER_H

#include "Filter.h"

class ReflectFilter : public Filter {
public:
    void apply(Image& img) override;
};

#endif // PIXLAB_REFLECT_FILTER_H
