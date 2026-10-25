#include "Image.h"
#include "BMPHandler.h"
#include "FilterFactory.h"
#include "GrayscaleFilter.h"
#include "SepiaFilter.h"
#include "BlurFilter.h"
#include "ReflectFilter.h"
#include "PixelateFilter.h"
#include "VignetteFilter.h"
#include "UnsharpMaskEnhancer.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

int main() {
    std::cout << "[PixLab Tests] Running OOP C++ Filter Test Suite...\n";

    // 1. Test Encapsulated Image bounds checking
    Image img(4, 4);
    assert(img.getWidth() == 4);
    assert(img.getHeight() == 4);

    bool threw = false;
    try {
        (void)img.at(4, 4);
    } catch (const std::out_of_range&) {
        threw = true;
    }
    assert(threw);
    std::cout << "  ✓ Image bounds checking passed.\n";

    // 2. Test BMP Serialization and Deserialization Round-trip
    img.at(0, 0) = {255, 0, 0};   // Blue
    img.at(0, 1) = {0, 255, 0};   // Green
    img.at(1, 0) = {0, 0, 255};   // Red
    img.at(1, 1) = {255, 255, 255}; // White

    std::vector<uint8_t> bmpBytes = BMPHandler::serialize(img);
    assert(bmpBytes.size() >= 54);
    Image reloaded = BMPHandler::deserialize(bmpBytes.data(), bmpBytes.size());
    assert(reloaded.getWidth() == 4 && reloaded.getHeight() == 4);
    assert(reloaded.at(0, 0).b == 255 && reloaded.at(0, 0).g == 0 && reloaded.at(0, 0).r == 0);
    std::cout << "  ✓ BMP Handler serialize/deserialize round-trip passed.\n";

    // 3. Test GrayscaleFilter
    Image grayTest(2, 2);
    grayTest.at(0, 0) = {30, 20, 10}; // B=30, G=20, R=10
    GrayscaleFilter grayFilter;
    grayFilter.apply(grayTest);
    uint8_t expectedGray = static_cast<uint8_t>(std::round(0.299 * 10 + 0.587 * 20 + 0.114 * 30));
    assert(grayTest.at(0, 0).r == expectedGray);
    assert(grayTest.at(0, 0).g == expectedGray);
    assert(grayTest.at(0, 0).b == expectedGray);
    std::cout << "  ✓ GrayscaleFilter (Rec. 601 Luminance) passed.\n";

    // 4. Test ReflectFilter
    Image reflectTest(2, 1);
    reflectTest.at(0, 0) = {10, 20, 30};
    reflectTest.at(0, 1) = {40, 50, 60};
    ReflectFilter reflectFilter;
    reflectFilter.apply(reflectTest);
    assert(reflectTest.at(0, 0).b == 40 && reflectTest.at(0, 1).b == 10);
    std::cout << "  ✓ ReflectFilter (Horizontal reflection) passed.\n";

    // 5. Test FilterFactory Polymorphism
    for (int typeId = 1; typeId <= 7; ++typeId) {
        std::unique_ptr<Filter> filter = FilterFactory::create(typeId, 50.0f);
        assert(filter != nullptr);
        Image testCanvas(8, 8);
        filter->apply(testCanvas);
    }
    std::cout << "  ✓ FilterFactory polymorphic instantiation (Filters 1-7) passed.\n";

    // 6. Test UnsharpMaskEnhancer Multi-Stage Clarity Engine
    Image enhanceTest(16, 16);
    for (int r = 0; r < 16; ++r) {
        for (int c = 0; c < 16; ++c) {
            enhanceTest.at(r, c) = {static_cast<uint8_t>(r * 15), static_cast<uint8_t>(c * 15), 128};
        }
    }
    UnsharpMaskEnhancer enhancer(2.2f, 0.18f, 1.18f);
    enhancer.apply(enhanceTest);
    assert(enhanceTest.getWidth() == 16 && enhanceTest.getHeight() == 16);
    std::cout << "  ✓ UnsharpMaskEnhancer multi-stage execution passed.\n";

    std::cout << "\nAll PixLab C++ OOP tests successfully passed!\n";
    return 0;
}
