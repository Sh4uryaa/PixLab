#ifndef PIXLAB_BMP_H
#define PIXLAB_BMP_H

#include <cstdint>

using BYTE = std::uint8_t;
using DWORD = std::uint32_t;
using LONG = std::int32_t;
using WORD = std::uint16_t;

#pragma pack(push, 1)
struct BITMAPFILEHEADER {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
};

struct BITMAPINFOHEADER {
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
};

struct RGBTRIPLE {
    BYTE rgbtBlue;
    BYTE rgbtGreen;
    BYTE rgbtRed;
};
#pragma pack(pop)

static_assert(sizeof(BITMAPFILEHEADER) == 14, "BMP file header must be 14 bytes");
static_assert(sizeof(BITMAPINFOHEADER) == 40, "BMP info header must be 40 bytes");
static_assert(sizeof(RGBTRIPLE) == 3, "BMP pixel must be 3 bytes");

#endif
