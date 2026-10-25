#include "ReflectFilter.h"
#include <utility>

void ReflectFilter::apply(Image& img) {
    const int width = img.getWidth();
    const int height = img.getHeight();

    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width / 2; ++col) {
            std::swap(img.at(row, col), img.at(row, width - 1 - col));
        }
    }
}
