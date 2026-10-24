#include "Image.h"

#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>

namespace {
void setError(std::string* out, const std::string& message) {
    if (out) *out = message;
}

bool readExact(std::ifstream& in, char* data, std::streamsize size) {
    in.read(data, size);
    return in.good() || in.gcount() == size;
}

bool writeExact(std::ofstream& out, const char* data, std::streamsize size) {
    out.write(data, size);
    return static_cast<bool>(out);
}
}

bool Image::load(const std::string& filename, std::string* error) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) {
        setError(error, "Could not open input file: " + filename);
        return false;
    }

    BITMAPFILEHEADER fileHeader{};
    BITMAPINFOHEADER infoHeader{};
    if (!readExact(in, reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader)) ||
        !readExact(in, reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader))) {
        setError(error, "File is too small to contain a BMP header.");
        return false;
    }

    if (fileHeader.bfType != 0x4D42 || fileHeader.bfOffBits != 54 ||
        infoHeader.biSize != 40 || infoHeader.biPlanes != 1 ||
        infoHeader.biBitCount != 24 || infoHeader.biCompression != 0) {
        setError(error, "Unsupported BMP. Expected a 24-bit, uncompressed BMP with a 54-byte header.");
        return false;
    }
    if (infoHeader.biWidth <= 0 || infoHeader.biHeight == 0 ||
        infoHeader.biHeight == std::numeric_limits<LONG>::min()) {
        setError(error, "Invalid BMP dimensions.");
        return false;
    }

    const auto width = static_cast<std::uint64_t>(infoHeader.biWidth);
    const auto height = static_cast<std::uint64_t>(infoHeader.biHeight < 0
        ? -static_cast<std::int64_t>(infoHeader.biHeight) : infoHeader.biHeight);
    if (width > static_cast<std::uint64_t>(std::numeric_limits<int>::max()) ||
        height > static_cast<std::uint64_t>(std::numeric_limits<int>::max()) ||
        width * height > 200000000ULL) {
        setError(error, "BMP dimensions are too large.");
        return false;
    }

    const int widthInt = static_cast<int>(width);
    const int heightInt = static_cast<int>(height);
    const int padding = (4 - (widthInt * static_cast<int>(sizeof(RGBTRIPLE))) % 4) % 4;
    std::vector<std::vector<RGBTRIPLE>> pixels(
        heightInt, std::vector<RGBTRIPLE>(widthInt));

    for (int row = 0; row < heightInt; ++row) {
        if (!readExact(in, reinterpret_cast<char*>(pixels[row].data()),
                       static_cast<std::streamsize>(widthInt * sizeof(RGBTRIPLE)))) {
            setError(error, "Unexpected end of file while reading BMP pixels.");
            return false;
        }
        char pad[3]{};
        if (padding && !readExact(in, pad, padding)) {
            setError(error, "Unexpected end of file while reading BMP row padding.");
            return false;
        }
    }

    fileHeader_ = fileHeader;
    infoHeader_ = infoHeader;
    width_ = widthInt;
    height_ = heightInt;
    pixels_ = std::move(pixels);
    return true;
}

bool Image::save(const std::string& filename, std::string* error) const {
    if (!isLoaded()) {
        setError(error, "No image is loaded.");
        return false;
    }
    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        setError(error, "Could not create output file: " + filename);
        return false;
    }

    BITMAPFILEHEADER fileHeader = fileHeader_;
    BITMAPINFOHEADER infoHeader = infoHeader_;
    const std::uint32_t padding = static_cast<std::uint32_t>(
        (4 - (width_ * static_cast<int>(sizeof(RGBTRIPLE))) % 4) % 4);
    const std::uint64_t pixelBytes =
        static_cast<std::uint64_t>(height_) * (static_cast<std::uint64_t>(width_) * 3 + padding);
    if (pixelBytes + 54 > std::numeric_limits<DWORD>::max()) {
        setError(error, "Image is too large to save as a BMP.");
        return false;
    }
    fileHeader.bfSize = static_cast<DWORD>(54 + pixelBytes);
    fileHeader.bfOffBits = 54;
    infoHeader.biSizeImage = static_cast<DWORD>(pixelBytes);

    if (!writeExact(out, reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader)) ||
        !writeExact(out, reinterpret_cast<const char*>(&infoHeader), sizeof(infoHeader))) {
        setError(error, "Failed writing BMP headers.");
        return false;
    }

    const char pad[3] = {0, 0, 0};
    for (const auto& row : pixels_) {
        if (!writeExact(out, reinterpret_cast<const char*>(row.data()),
                        static_cast<std::streamsize>(row.size() * sizeof(RGBTRIPLE))) ||
            (padding && !writeExact(out, pad, padding))) {
            setError(error, "Failed writing BMP pixels.");
            return false;
        }
    }
    out.flush();
    if (!out) {
        setError(error, "Failed finalizing output BMP.");
        return false;
    }
    return true;
}

RGBTRIPLE Image::getPixel(int row, int col) const {
    if (row < 0 || row >= height_ || col < 0 || col >= width_)
        throw std::out_of_range("Image pixel coordinates are out of range.");
    return pixels_[row][col];
}

void Image::setPixel(int row, int col, RGBTRIPLE value) {
    if (row < 0 || row >= height_ || col < 0 || col >= width_)
        throw std::out_of_range("Image pixel coordinates are out of range.");
    pixels_[row][col] = value;
}
