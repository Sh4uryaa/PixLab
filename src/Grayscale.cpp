#include "Grayscale.h"
#include "Image.h"
#include <cmath>

void Grayscale::apply(Image& image) const {
    for (int row = 0; row < image.getHeight(); ++row) {
        for (int col = 0; col < image.getWidth(); ++col) {
            RGBTRIPLE p = image.getPixel(row, col);
            const int average = static_cast<int>(std::round(
                (p.rgbtRed + p.rgbtGreen + p.rgbtBlue) / 3.0));
            p.rgbtRed = p.rgbtGreen = p.rgbtBlue = static_cast<BYTE>(average);
            image.setPixel(row, col, p);
        }
    }
}
