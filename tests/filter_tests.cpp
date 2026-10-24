#include "../src/Blur.h"
#include "../src/Grayscale.h"
#include "../src/Reflect.h"
#include "../src/Sepia.h"
#include "../src/Image.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <algorithm>
#include <stdexcept>
#include <vector>

// Tests use a tiny BMP written by Image::save so padding and round-trip are exercised.
static Image makeImage(int width, int height) {
    // Test fixture writer: build a BMP file header through a temporary BMP, then reload it.
    const char* fixture = "pixlab_test_fixture.bmp";
    std::ofstream out(fixture, std::ios::binary);
    BITMAPFILEHEADER bf{};
    BITMAPINFOHEADER bi{};
    const int pad = (4 - (width * 3) % 4) % 4;
    const std::uint32_t size = 54 + height * (width * 3 + pad);
    bf.bfType = 0x4D42; bf.bfSize = size; bf.bfOffBits = 54;
    bi.biSize = 40; bi.biWidth = width; bi.biHeight = height;
    bi.biPlanes = 1; bi.biBitCount = 24; bi.biCompression = 0; bi.biSizeImage = size - 54;
    out.write(reinterpret_cast<char*>(&bf), sizeof(bf));
    out.write(reinterpret_cast<char*>(&bi), sizeof(bi));
    const char padding[3] = {0,0,0};
    for (int y=0; y<height; ++y) {
        for (int x=0; x<width; ++x) {
            RGBTRIPLE p{static_cast<BYTE>(10+x), static_cast<BYTE>(20+y), static_cast<BYTE>(30+x+y)};
            out.write(reinterpret_cast<char*>(&p), sizeof(p));
        }
        out.write(padding, pad);
    }
    out.close();
    Image img;
    std::string error;
    assert(img.load(fixture, &error) && error.empty());
    return img;
}

int main() {
    Image gray = makeImage(2, 2);
    RGBTRIPLE p = gray.getPixel(0,0);
    const int expected = static_cast<int>(std::round((30+20+10)/3.0));
    Grayscale().apply(gray);
    p = gray.getPixel(0,0);
    assert(p.rgbtRed == expected && p.rgbtGreen == expected && p.rgbtBlue == expected);

    Image reflected = makeImage(2, 1);
    const RGBTRIPLE left = reflected.getPixel(0,0);
    const RGBTRIPLE right = reflected.getPixel(0,1);
    Reflect().apply(reflected);
    assert(reflected.getPixel(0,0).rgbtRed == right.rgbtRed);
    assert(reflected.getPixel(0,1).rgbtRed == left.rgbtRed);

    Image blur = makeImage(1, 1);
    const RGBTRIPLE before = blur.getPixel(0,0);
    Blur().apply(blur);
    assert(blur.getPixel(0,0).rgbtRed == before.rgbtRed);
    assert(blur.getPixel(0,0).rgbtGreen == before.rgbtGreen);
    assert(blur.getPixel(0,0).rgbtBlue == before.rgbtBlue);

    Image sepia = makeImage(1, 1);
    const RGBTRIPLE orig = sepia.getPixel(0,0);
    Sepia().apply(sepia);
    const RGBTRIPLE transformed = sepia.getPixel(0,0);
    assert(transformed.rgbtRed == std::min(255, static_cast<int>(std::round(.393*orig.rgbtRed + .769*orig.rgbtGreen + .189*orig.rgbtBlue))));

    assert(sepia.save("pixlab_test_output.bmp"));
    Image reloaded;
    assert(reloaded.load("pixlab_test_output.bmp"));
    assert(reloaded.getWidth() == 1 && reloaded.getHeight() == 1);
    assert(reloaded.getPixel(0,0).rgbtRed == transformed.rgbtRed);

    bool threw = false;
    try { (void)reloaded.getPixel(4,4); } catch (const std::out_of_range&) { threw = true; }
    assert(threw);
    std::remove("pixlab_test_fixture.bmp");
    std::remove("pixlab_test_output.bmp");
    std::cout << "All PixLab filter tests passed.\n";
}
