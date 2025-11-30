#pragma once
#include "../IEffect.h"

/**
 * @class SolarizeEffect
 * @brief Inverts pixel colors only if they exceed a specific threshold.
 * Creates a "burned film" or psychedelic look.
 */
class SolarizeEffect : public IEffect {
public:
    void apply(std::vector<Pixel>& data, int imgWidth, int imgHeight, 
               const Region& region, const EffectParams& params) override {
        
        // Threshold is inverse of intensity (High intensity = low threshold = more effect)
        uint8_t threshold = static_cast<uint8_t>(255 - (params.intensity * 2.5));

        for (int y = region.y; y < region.y + region.height; ++y) {
            for (int x = region.x; x < region.x + region.width; ++x) {
                
                if (!isInsideBubble(x, y, params)) continue;

                int idx = (y * imgWidth) + x;
                Pixel& p = data[idx];

                // Logic: If channel > threshold, invert it. Else, keep it.
                if (p.r > threshold) p.r = 255 - p.r;
                if (p.g > threshold) p.g = 255 - p.g;
                if (p.b > threshold) p.b = 255 - p.b;
            }
        }
    }
};