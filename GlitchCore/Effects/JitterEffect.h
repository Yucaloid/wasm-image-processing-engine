#pragma once
#include "../IEffect.h"
#include <cstdlib>
#include <vector>
#include <algorithm>

/**
 * @class JitterEffect
 * @brief Divides the image into blocks and randomly displaces them.
 * Simulates data corruption or "datamoshing".
 */
class JitterEffect : public IEffect {
public:
    void apply(std::vector<Pixel>& data, int imgWidth, int imgHeight, 
               const Region& region, const EffectParams& params) override {
        
        // Copy original buffer to read source blocks safely
        std::vector<Pixel> source = data;
        
        int blockSize = 10; // Fixed block size for the glitch look
        int shiftPower = static_cast<int>(params.intensity); // Max displacement in pixels

        for (int y = region.y; y < region.y + region.height; y += blockSize) {
            for (int x = region.x; x < region.x + region.width; x += blockSize) {
                
                if (!isInsideBubble(x, y, params)) continue;

                // Calculate a random offset vector for this specific block
                int offsetX = (std::rand() % std::max(1, shiftPower)) - (shiftPower / 2);
                int offsetY = (std::rand() % std::max(1, shiftPower)) - (shiftPower / 2);

                // Copy the displaced block from source to destination
                for (int by = 0; by < blockSize; ++by) {
                    for (int bx = 0; bx < blockSize; ++bx) {
                        int destX = x + bx;
                        int destY = y + by;
                        
                        // Calculate source coordinates (Inverse mapping)
                        int srcX = destX + offsetX;
                        int srcY = destY + offsetY;

                        // Boundary checks for destination
                        if (destX >= imgWidth || destY >= imgHeight) continue;
                        
                        // Clamp source coordinates to keep them valid (Wrap or Clamp)
                        // Using Clamp here to repeat edges (smear effect)
                        srcX = std::max(0, std::min(imgWidth - 1, srcX));
                        srcY = std::max(0, std::min(imgHeight - 1, srcY));

                        data[destY * imgWidth + destX] = source[srcY * imgWidth + srcX];
                    }
                }
            }
        }
    }
};