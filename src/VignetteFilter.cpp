#include "VignetteFilter.h"
#include <algorithm>
#include <cmath>

void VignetteFilter::apply(Image& img) {
    const int width = img.getWidth();
    const int height = img.getHeight();

    const double cx = (width - 1) / 2.0;
    const double cy = (height - 1) / 2.0;
    const double maxDist = std::sqrt(cx * cx + cy * cy);

    if (maxDist < 1e-5) return;

    for (int row = 0; row < height; ++row) {
        double dy = row - cy;
        for (int col = 0; col < width; ++col) {
            double dx = col - cx;
            double dist = std::sqrt(dx * dx + dy * dy);
            double normDist = dist / maxDist;

            // Smooth quadratic radial falloff towards corners
            double factor = 1.0 - (normDist * normDist * 0.75);
            if (factor < 0.0) factor = 0.0;
            if (factor > 1.0) factor = 1.0;

            RGBTRIPLE& p = img.at(row, col);
            p.r = clamp(static_cast<int>(std::round(p.r * factor)));
            p.g = clamp(static_cast<int>(std::round(p.g * factor)));
            p.b = clamp(static_cast<int>(std::round(p.b * factor)));
        }
    }
}
