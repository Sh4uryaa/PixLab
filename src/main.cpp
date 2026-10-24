#include "Image.h"
#include "Blur.h"
#include "Grayscale.h"
#include "Reflect.h"
#include "Sepia.h"

#include <iostream>
#include <memory>
#include <string>

namespace {
void usage(const char* program) {
    std::cerr << "Usage: " << program << " [-b|-g|-r|-s] input.bmp output.bmp\n"
              << "  -b  blur\n  -g  grayscale\n  -r  horizontal reflection\n  -s  sepia\n";
}
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        usage(argv[0]);
        return 2;
    }
    const std::string flag = argv[1];
    std::unique_ptr<Filter> filter;
    if (flag == "-b") filter = std::make_unique<Blur>();
    else if (flag == "-g") filter = std::make_unique<Grayscale>();
    else if (flag == "-r") filter = std::make_unique<Reflect>();
    else if (flag == "-s") filter = std::make_unique<Sepia>();
    else {
        std::cerr << "Invalid filter: " << flag << "\n";
        usage(argv[0]);
        return 2;
    }

    Image image;
    std::string error;
    if (!image.load(argv[2], &error)) {
        std::cerr << error << '\n';
        return 3;
    }
    filter->apply(image);
    if (!image.save(argv[3], &error)) {
        std::cerr << error << '\n';
        return 4;
    }
    return 0;
}
