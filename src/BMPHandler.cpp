#include "BMPHandler.h"
#include <cstring>
#include <cmath>
#include <stdexcept>
#include <string>

Image BMPHandler::deserialize(const uint8_t* data, size_t size) {
    const size_t headerSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    if (!data || size < headerSize) {
        throw std::invalid_argument("BMP buffer too small to contain valid headers.");
    }

    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    std::memcpy(&fileHeader, data, sizeof(BITMAPFILEHEADER));
    std::memcpy(&infoHeader, data + sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));

    // Validate Windows BMP magic identifier ("BM" -> 0x4D42)
    if (fileHeader.bfType != 0x4D42) {
        throw std::runtime_error("Invalid BMP magic identifier (expected 0x4D42 'BM').");
    }

    // PixLab strictly supports standard 24-bit uncompressed BMP (BI_RGB)
    if (infoHeader.biBitCount != 24) {
        throw std::runtime_error("Unsupported bit depth: PixLab requires 24-bit BMP.");
    }

    if (infoHeader.biCompression != 0) {
        throw std::runtime_error("Unsupported compression: PixLab requires uncompressed BI_RGB.");
    }

    if (infoHeader.biPlanes != 1) {
        throw std::runtime_error("Invalid number of planes in BMP header.");
    }

    int width = infoHeader.biWidth;
    int height = std::abs(infoHeader.biHeight);
    bool isTopDown = (infoHeader.biHeight < 0);

    if (width <= 0 || height <= 0) {
        throw std::runtime_error("Invalid image dimensions in BMP header.");
    }

    // Windows BMP scanlines are padded to 4-byte boundaries
    int padding = (4 - (width * 3) % 4) % 4;
    size_t rowStride = static_cast<size_t>(width * 3 + padding);
    size_t requiredDataSize = static_cast<size_t>(fileHeader.bfOffBits) + rowStride * height;

    if (size < requiredDataSize) {
        throw std::runtime_error("Corrupted BMP: file payload smaller than expected pixel data.");
    }

    Image img(width, height);
    const uint8_t* pixelData = data + fileHeader.bfOffBits;

    for (int i = 0; i < height; ++i) {
        // Standard BMP (positive height) is stored bottom-up; negative is top-down
        int targetRow = isTopDown ? i : (height - 1 - i);
        const uint8_t* rowPtr = pixelData + (i * rowStride);

        for (int col = 0; col < width; ++col) {
            RGBTRIPLE& p = img.at(targetRow, col);
            // BMP 24-bit stores in BGR order: Blue, Green, Red
            p.b = rowPtr[col * 3 + 0];
            p.g = rowPtr[col * 3 + 1];
            p.r = rowPtr[col * 3 + 2];
        }
    }

    return img;
}

std::vector<uint8_t> BMPHandler::serialize(const Image& img) {
    int width = img.getWidth();
    int height = img.getHeight();

    int padding = (4 - (width * 3) % 4) % 4;
    uint32_t rowStride = static_cast<uint32_t>(width * 3 + padding);
    uint32_t pixelArraySize = rowStride * static_cast<uint32_t>(height);
    uint32_t totalFileSize = static_cast<uint32_t>(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)) + pixelArraySize;

    BITMAPFILEHEADER fileHeader{};
    fileHeader.bfType = 0x4D42; // "BM"
    fileHeader.bfSize = totalFileSize;
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    BITMAPINFOHEADER infoHeader{};
    infoHeader.biSize = sizeof(BITMAPINFOHEADER);
    infoHeader.biWidth = width;
    infoHeader.biHeight = height; // Positive indicates standard bottom-up
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 24;   // 24-bit uncompressed RGB
    infoHeader.biCompression = 0; // BI_RGB
    infoHeader.biSizeImage = pixelArraySize;
    infoHeader.biXPelsPerMeter = 2835; // ~72 DPI
    infoHeader.biYPelsPerMeter = 2835;
    infoHeader.biClrUsed = 0;
    infoHeader.biClrImportant = 0;

    std::vector<uint8_t> buffer(totalFileSize, 0);

    // Write headers
    std::memcpy(buffer.data(), &fileHeader, sizeof(BITMAPFILEHEADER));
    std::memcpy(buffer.data() + sizeof(BITMAPFILEHEADER), &infoHeader, sizeof(BITMAPINFOHEADER));

    uint8_t* pixelData = buffer.data() + fileHeader.bfOffBits;

    // Write scanlines bottom-to-top (row = height - 1 down to 0)
    for (int i = 0; i < height; ++i) {
        int sourceRow = height - 1 - i;
        uint8_t* rowPtr = pixelData + (i * rowStride);

        for (int col = 0; col < width; ++col) {
            const RGBTRIPLE& p = img.at(sourceRow, col);
            rowPtr[col * 3 + 0] = p.b;
            rowPtr[col * 3 + 1] = p.g;
            rowPtr[col * 3 + 2] = p.r;
        }

        // Fill scanline padding bytes with 0x00
        for (int pad = 0; pad < padding; ++pad) {
            rowPtr[width * 3 + pad] = 0x00;
        }
    }

    return buffer;
}
