#ifndef PIXLAB_BMP_HANDLER_H
#define PIXLAB_BMP_HANDLER_H

#include "Image.h"
#include <cstdint>
#include <cstddef>
#include <vector>

#pragma pack(push, 1)
struct BITMAPFILEHEADER {
    uint16_t bfType;      // File type magic number, must be 0x4D42 ("BM")
    uint32_t bfSize;      // Size of the BMP file in bytes
    uint16_t bfReserved1; // Reserved, must be 0
    uint16_t bfReserved2; // Reserved, must be 0
    uint32_t bfOffBits;   // Byte offset from beginning to the pixel data
};

struct BITMAPINFOHEADER {
    uint32_t biSize;          // Size of info header in bytes (40)
    int32_t  biWidth;         // Image width in pixels
    int32_t  biHeight;        // Image height in pixels (signed: positive = bottom-up, negative = top-down)
    uint16_t biPlanes;        // Number of color planes, must be 1
    uint16_t biBitCount;      // Number of bits per pixel, must be 24 for uncompressed RGB
    uint32_t biCompression;   // Compression method, 0 = BI_RGB (uncompressed)
    uint32_t biSizeImage;     // Image data size in bytes (may be 0 for BI_RGB)
    int32_t  biXPelsPerMeter; // Horizontal resolution (pixels per meter)
    int32_t  biYPelsPerMeter; // Vertical resolution (pixels per meter)
    uint32_t biClrUsed;       // Number of color indices used
    uint32_t biClrImportant;  // Number of important color indices
};
#pragma pack(pop)

class BMPHandler {
public:
    // Deserializes a raw 24-bit BMP byte buffer from RAM into an Image object
    static Image deserialize(const uint8_t* data, size_t size);

    // Serializes an Image object back into standard 24-bit BMP byte format in RAM
    static std::vector<uint8_t> serialize(const Image& img);
};

#endif // PIXLAB_BMP_HANDLER_H
