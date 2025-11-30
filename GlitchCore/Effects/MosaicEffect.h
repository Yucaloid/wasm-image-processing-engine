#pragma once
#include "../IEffect.h"
#include <algorithm>

/**
 * @class MosaicEffect
 * @brief Reduces the resolution of the image area to create a pixelated look.
 * Useful for retro aesthetics or censorship effects.
 */
class MosaicEffect : public IEffect {
public:
    void apply(std::vector<Pixel>& data, int imgWidth, int imgHeight, 
               const Region& region, const EffectParams& params) override {
        
        // Define block size based on intensity. Minimum 1px, max 50px approx.
        int blockSize = std::max(1, static_cast<int>(params.intensity / 2));

        // Iterate through the grid in steps of 'blockSize'
        for (int y = region.y; y < region.y + region.height; y += blockSize) {
            for (int x = region.x; x < region.x + region.width; x += blockSize) {
                
                // Check if the top-left corner of the block is inside the bubble
                if (!isInsideBubble(x, y, params)) continue;

                // 1. Sample the color from the first pixel of the block
                int index = (y * imgWidth) + x;
                Pixel sample = data[index];

                // 2. Fill the entire block with that sample color
                for (int by = 0; by < blockSize; ++by) {
                    for (int bx = 0; bx < blockSize; ++bx) {
                        int pX = x + bx;
                        int pY = y + by;

                        // Vital boundary check to prevent segmentation faults
                        if (pX >= imgWidth || pY >= imgHeight) continue;
                        
                        // Note: We skip the circular check per-pixel here for performance 
                        // and to keep the "blocky" aesthetic at the edges.

                        data[pY * imgWidth + pX] = sample;
                    }
                }
            }
        }
    }
};