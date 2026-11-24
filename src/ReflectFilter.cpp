#include "ReflectFilter.h"
#include <utility>

void ReflectFilter::apply(Image& img) {
    const int width = img.getWidth();
    const int height = img.getHeight();
    const int halfWidth = width / 2;

    for (int row = 0; row < height; ++row) {
        RGBTRIPLE* rowPtr = img.row(row);
        for (int col = 0; col < halfWidth; ++col) {
            std::swap(rowPtr[col], rowPtr[width - 1 - col]);
        }
    }
}
