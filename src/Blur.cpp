#include "Blur.h"
#include "Image.h"
#include <cmath>
#include <vector>

void Blur::apply(Image& image) const {
    // Snapshot first: every output pixel must use the original neighborhood.
    const int height = image.getHeight();
    const int width = image.getWidth();
    std::vector<std::vector<RGBTRIPLE>> original(
        height, std::vector<RGBTRIPLE>(width));
    for (int row = 0; row < height; ++row)
        for (int col = 0; col < width; ++col)
            original[row][col] = image.getPixel(row, col);

    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            int red = 0, green = 0, blue = 0, count = 0;
            for (int y = row - 1; y <= row + 1; ++y) {
                for (int x = col - 1; x <= col + 1; ++x) {
                    if (y < 0 || y >= height || x < 0 || x >= width) continue;
                    red += original[y][x].rgbtRed;
                    green += original[y][x].rgbtGreen;
                    blue += original[y][x].rgbtBlue;
                    ++count;
                }
            }
            RGBTRIPLE p{};
            p.rgbtRed = static_cast<BYTE>(std::round(red / static_cast<double>(count)));
            p.rgbtGreen = static_cast<BYTE>(std::round(green / static_cast<double>(count)));
            p.rgbtBlue = static_cast<BYTE>(std::round(blue / static_cast<double>(count)));
            image.setPixel(row, col, p);
        }
    }
}
