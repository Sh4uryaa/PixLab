#include "Sepia.h"
#include "Image.h"
#include <algorithm>
#include <cmath>

void Sepia::apply(Image& image) const {
    for (int row = 0; row < image.getHeight(); ++row) {
        for (int col = 0; col < image.getWidth(); ++col) {
            RGBTRIPLE p = image.getPixel(row, col);
            const int red = p.rgbtRed;
            const int green = p.rgbtGreen;
            const int blue = p.rgbtBlue;
            p.rgbtRed = static_cast<BYTE>(std::clamp(static_cast<int>(
                std::round(.393 * red + .769 * green + .189 * blue)), 0, 255));
            p.rgbtGreen = static_cast<BYTE>(std::clamp(static_cast<int>(
                std::round(.349 * red + .686 * green + .168 * blue)), 0, 255));
            p.rgbtBlue = static_cast<BYTE>(std::clamp(static_cast<int>(
                std::round(.272 * red + .534 * green + .131 * blue)), 0, 255));
            image.setPixel(row, col, p);
        }
    }
}
