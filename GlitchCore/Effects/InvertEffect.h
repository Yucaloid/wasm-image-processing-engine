#pragma once
#include "../IEffect.h"

/**
 * @class InvertEffect
 * @brief Simple negative effect. Inverts RGB channels.
 */
class InvertEffect : public IEffect {
public:
    void apply(std::vector<Pixel>& data, int imgWidth, int imgHeight, 
               const Region& region, const EffectParams& params) override {
        
        // Loop ONLY through the region of interest
        for (int y = region.y; y < region.y + region.height; ++y) {
            for (int x = region.x; x < region.x + region.width; ++x) {
                
                // Check Bubble Mask
                if (!isInsideBubble(x, y, params)) continue;

                int index = (y * imgWidth) + x;
                Pixel& p = data[index];

                // Logic: Invert colors based on intensity
                // If intensity is 100 (1.0), fully invert. If 0, do nothing.
                float factor = params.intensity / 100.0f;
                
                p.r = static_cast<uint8_t>(p.r * (1 - factor) + (255 - p.r) * factor);
                p.g = static_cast<uint8_t>(p.g * (1 - factor) + (255 - p.g) * factor);
                p.b = static_cast<uint8_t>(p.b * (1 - factor) + (255 - p.b) * factor);
            }
        }
    }
};