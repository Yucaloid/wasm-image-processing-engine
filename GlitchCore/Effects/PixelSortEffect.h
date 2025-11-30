#pragma once
#include "../IEffect.h"
#include <algorithm> // for std::sort
#include <cmath>

/**
 * @class PixelSortEffect
 * @brief Sorts pixels by luminance within vertical columns.
 * Supports circular masking to create a "melting bubble" effect.
 */
class PixelSortEffect : public IEffect {
public:
    void apply(std::vector<Pixel>& data, int imgWidth, int imgHeight, 
               const Region& region, const EffectParams& params) override {
        
        // We iterate column by column (X axis) within the region
        for (int x = region.x; x < region.x + region.width; ++x) {
            
            int startY = region.y;
            int endY = region.y + region.height;

            // --- BUBBLE MATH OPTIMIZATION ---
            // If we are using a circle mask, we calculate the precise vertical span 
            // of the circle at this specific X coordinate.
            if (params.useCircleMask) {
                int dx = x - params.centerX;
                int distSq = dx * dx;
                int radiusSq = params.radius * params.radius;

                // If this column is outside the circle's width, skip it completely.
                if (distSq > radiusSq) continue;

                // Pythagorean theorem to find the height of the chord at x
                // y = sqrt(r^2 - x^2)
                int ySpan = static_cast<int>(std::sqrt(radiusSq - distSq));
                
                // Clamping to image boundaries
                startY = std::max(0, params.centerY - ySpan);
                endY = std::min(imgHeight, params.centerY + ySpan);
            }

            // Safety check
            if (startY >= endY) continue;

            // --- SORTING PROCESS ---
            processColumn(data, imgWidth, x, startY, endY, params.intensity);
        }
    }

private:
    /**
     * @brief Extracts, sorts, and writes back a column segment.
     */
    void processColumn(std::vector<Pixel>& data, int width, int x, int startY, int endY, float intensity) {
        int length = endY - startY;
        if (length <= 1) return;

        // 1. Extract pixels into a temporary vector
        std::vector<Pixel> columnStrip;
        columnStrip.reserve(length);
        
        for (int y = startY; y < endY; ++y) {
            columnStrip.push_back(data[y * width + x]);
        }

        // 2. Sort the strip based on Luminance
        // Intensity > 50 sorts ascending, < 50 sorts descending (optional feature)
        if (intensity > 0) {
            std::sort(columnStrip.begin(), columnStrip.end(), [](const Pixel& a, const Pixel& b) {
                return a.getLuminance() < b.getLuminance(); // Darkest at top
            });
        }

        // 3. Write back to the main buffer
        for (int i = 0; i < length; ++i) {
            data[(startY + i) * width + x] = columnStrip[i];
        }
    }
};