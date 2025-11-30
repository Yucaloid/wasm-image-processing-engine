#pragma once
#include "../IEffect.h"
#include <cstdlib>
#include <vector>
#include <algorithm>

/**
 * @class ScanlineEffect
 * @brief Horizontally shifts individual rows randomly.
 * Simulates VHS tracking errors or signal interference.
 */
class ScanlineEffect : public IEffect {
public:
    void apply(std::vector<Pixel>& data, int imgWidth, int imgHeight, 
               const Region& region, const EffectParams& params) override {
        
        std::vector<Pixel> source = data;
        int maxShift = static_cast<int>(params.intensity);

        for (int y = region.y; y < region.y + region.height; ++y) {
            
            // Randomly decide if this line should be shifted.
            // 30% probability of shifting a line to create a noisy look.
            if ((std::rand() % 100) > 30) continue; 
            
            // Calculate random horizontal shift
            int shift = (std::rand() % std::max(1, maxShift)) - (maxShift / 2);

            for (int x = region.x; x < region.x + region.width; ++x) {
                if (!isInsideBubble(x, y, params)) continue;

                int srcX = x - shift;
                
                // Clamp horizontal coordinate to image bounds
                srcX = std::max(0, std::min(imgWidth - 1, srcX));

                data[y * imgWidth + x] = source[y * imgWidth + srcX];
            }
        }
    }
};