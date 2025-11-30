#pragma once
#include "../IEffect.h"
#include <algorithm>

/**
 * @class ChromaticEffect
 * @brief Simulates lens dispersion by separating RGB channels spatially.
 */
class ChromaticEffect : public IEffect {
public:
    void apply(std::vector<Pixel>& data, int imgWidth, int imgHeight, 
               const Region& region, const EffectParams& params) override {
        
        // Intensity defines the offset distance in pixels
        int offset = static_cast<int>(params.intensity);
        if (offset == 0) return;

        // Create a copy of the region to read original values from.
        // We need this because we are modifying the buffer in-place, 
        // and we don't want to read pixels we just modified.
        // NOTE: For max performance in a real engine, we would swap full buffers,
        // but for this localized effect, a temp buffer is fine.
        std::vector<Pixel> tempBuffer = data; 

        for (int y = region.y; y < region.y + region.height; ++y) {
            for (int x = region.x; x < region.x + region.width; ++x) {
                
                if (!isInsideBubble(x, y, params)) continue;

                int index = (y * imgWidth) + x;
                
                // Calculate neighbor indices with boundary checks (Clamp)
                int rX = std::max(0, std::min(imgWidth - 1, x - offset)); // Shift Red Left
                int bX = std::max(0, std::min(imgWidth - 1, x + offset)); // Shift Blue Right
                
                int rIndex = (y * imgWidth) + rX;
                int bIndex = (y * imgWidth) + bX;

                // Write to the main data buffer
                Pixel& p = data[index];
                
                // Construct the new pixel:
                // Red comes from the left, Blue from the right, Green stays center
                p.r = tempBuffer[rIndex].r;
                p.g = tempBuffer[index].g; // Green untouched
                p.b = tempBuffer[bIndex].b;
            }
        }
    }
};