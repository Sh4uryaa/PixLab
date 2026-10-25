#include "Image.h"
#include <string>

Image::Image(int width, int height)
    : m_width(width), m_height(height) {
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("Image dimensions must be positive non-zero integers.");
    }
    m_pixels.resize(static_cast<std::size_t>(width) * height);
}

RGBTRIPLE& Image::at(int row, int col) {
    if (row < 0 || row >= m_height || col < 0 || col >= m_width) {
        throw std::out_of_range("Coordinates (" + std::to_string(row) + ", " +
                                std::to_string(col) + ") out of image bounds [" +
                                std::to_string(m_height) + "x" + std::to_string(m_width) + "]");
    }
    return m_pixels[static_cast<std::size_t>(row) * m_width + col];
}

const RGBTRIPLE& Image::at(int row, int col) const {
    if (row < 0 || row >= m_height || col < 0 || col >= m_width) {
        throw std::out_of_range("Coordinates (" + std::to_string(row) + ", " +
                                std::to_string(col) + ") out of image bounds [" +
                                std::to_string(m_height) + "x" + std::to_string(m_width) + "]");
    }
    return m_pixels[static_cast<std::size_t>(row) * m_width + col];
}
