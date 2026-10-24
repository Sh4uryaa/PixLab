#include "Reflect.h"
#include "Image.h"
#include <utility>

void Reflect::apply(Image& image) const {
    for (int row = 0; row < image.getHeight(); ++row) {
        for (int col = 0; col < image.getWidth() / 2; ++col) {
            const int other = image.getWidth() - col - 1;
            const RGBTRIPLE left = image.getPixel(row, col);
            const RGBTRIPLE right = image.getPixel(row, other);
            image.setPixel(row, col, right);
            image.setPixel(row, other, left);
        }
    }
}
