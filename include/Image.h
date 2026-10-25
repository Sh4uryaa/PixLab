#ifndef PIXLAB_IMAGE_H
#define PIXLAB_IMAGE_H

#include <cstdint>
#include <vector>
#include <stdexcept>

#pragma pack(push, 1)
struct RGBTRIPLE {
    uint8_t b;
    uint8_t g;
    uint8_t r;
};
#pragma pack(pop)

class Image {
public:
    // Constructor initializes width, height and flat contiguous buffer
    Image(int width, int height);

    // Deep copy constructor and copy assignment operator for buffering
    Image(const Image& other) = default;
    Image& operator=(const Image& other) = default;

    // Move constructor and move assignment operator
    Image(Image&& other) noexcept = default;
    Image& operator=(Image&& other) noexcept = default;

    ~Image() = default;

    // Dimensional accessors
    int getWidth() const noexcept { return m_width; }
    int getHeight() const noexcept { return m_height; }

    // Bounds-checked 2D coordinate access (row, col)
    RGBTRIPLE& at(int row, int col);
    const RGBTRIPLE& at(int row, int col) const;

    // Direct access to contiguous pixel buffer
    std::vector<RGBTRIPLE>& getPixels() noexcept { return m_pixels; }
    const std::vector<RGBTRIPLE>& getPixels() const noexcept { return m_pixels; }

private:
    int m_width;
    int m_height;
    std::vector<RGBTRIPLE> m_pixels;
};

#endif // PIXLAB_IMAGE_H
