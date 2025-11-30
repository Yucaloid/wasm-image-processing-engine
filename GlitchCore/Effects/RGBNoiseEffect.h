#pragma once
#include "../IEffect.h"
#include <cstdlib>

/**
 * @class RGBNoiseEffect
 * @brief Adds random static noise independently to R, G, and B channels.
 */
class RGBNoiseEffect : public IEffect {
public:
    void apply(std::vector<Pixel>& data, int imgWidth, int imgHeight, 
               const Region& region, const EffectParams& params) override {
        
        int noiseLevel = static_cast<int>(params.intensity);

        for (int y = region.y; y < region.y + region.height; ++y) {
            for (int x = region.x; x < region.x + region.width; ++x) {
                
                if (!isInsideBubble(x, y, params)) continue;

                int idx = (y * imgWidth) + x;
                Pixel& p = data[idx];

                // Add random value between -noiseLevel and +noiseLevel per channel
                int nr = (std::rand() % (noiseLevel * 2)) - noiseLevel;
                int ng = (std::rand() % (noiseLevel * 2)) - noiseLevel;
                int nb = (std::rand() % (noiseLevel * 2)) - noiseLevel;

                p.r = static_cast<uint8_t>(std::max(0, std::min(255, p.r + nr)));
                p.g = static_cast<uint8_t>(std::max(0, std::min(255, p.g + ng)));
                p.b = static_cast<uint8_t>(std::max(0, std::min(255, p.b + nb)));
            }
        }
    }
};