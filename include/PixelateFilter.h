#ifndef PIXLAB_PIXELATE_FILTER_H
#define PIXLAB_PIXELATE_FILTER_H

#include "Filter.h"

class PixelateFilter : public Filter {
public:
    explicit PixelateFilter(int blockSize = 10);
    void apply(Image& img) override;

private:
    int m_blockSize;
};

#endif // PIXLAB_PIXELATE_FILTER_H
