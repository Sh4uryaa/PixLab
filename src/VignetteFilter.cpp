#include "VignetteFilter.h"
#include <algorithm>

void VignetteFilter::apply(Image& img) {
    const int width = img.getWidth();
    const int height = img.getHeight();

    const double cx = (width - 1) / 2.0;
    const double cy = (height - 1) / 2.0;
    const double maxDistSq = cx * cx + cy * cy;

    if (maxDistSq < 1e-10) return;

    // Eliminates sqrt entirely: normDist^2 = (dx^2 + dy^2) / maxDistSq
    const double invMaxDistSq = 0.75 / maxDistSq;

    for (int row = 0; row < height; ++row) {
        const double dy = row - cy;
        const double dySq = dy * dy;
        RGBTRIPLE* rowPtr = img.row(row);

        for (int col = 0; col < width; ++col) {
            const double dx = col - cx;
            const double distSq = dx * dx + dySq;

            // Quadratic radial falloff without sqrt
            double factor = 1.0 - (distSq * invMaxDistSq);
            if (factor < 0.0) factor = 0.0;
            if (factor > 1.0) factor = 1.0;

            const float f = static_cast<float>(factor);
            rowPtr[col].r = static_cast<uint8_t>(rowPtr[col].r * f + 0.5f);
            rowPtr[col].g = static_cast<uint8_t>(rowPtr[col].g * f + 0.5f);
            rowPtr[col].b = static_cast<uint8_t>(rowPtr[col].b * f + 0.5f);
        }
    }
}
