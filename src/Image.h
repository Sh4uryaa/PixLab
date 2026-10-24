#ifndef PIXLAB_IMAGE_H
#define PIXLAB_IMAGE_H

#include "bmp.h"
#include <string>
#include <vector>

class Image {
public:
    Image() = default;

    bool load(const std::string& filename, std::string* error = nullptr);
    bool save(const std::string& filename, std::string* error = nullptr) const;

    int getWidth() const noexcept { return width_; }
    int getHeight() const noexcept { return height_; }

    RGBTRIPLE getPixel(int row, int col) const;
    void setPixel(int row, int col, RGBTRIPLE value);
    bool isLoaded() const noexcept { return !pixels_.empty(); }

private:
    BITMAPFILEHEADER fileHeader_{};
    BITMAPINFOHEADER infoHeader_{};
    int width_ = 0;
    int height_ = 0;
    // Pixels are kept in file scanline order so loading/saving preserves BMP orientation.
    std::vector<std::vector<RGBTRIPLE>> pixels_;
};

#endif
