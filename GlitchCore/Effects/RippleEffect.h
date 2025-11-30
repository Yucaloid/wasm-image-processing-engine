#pragma once
#include "../IEffect.h"
#include <cmath>

/**
 * @class RippleEffect
 * @brief Creates a sinusoidal ripple distortion emanating from the center.
 */
class RippleEffect : public IEffect {
public:
    void apply(std::vector<Pixel>& data, int imgWidth, int imgHeight, 
               const Region& region, const EffectParams& params) override {
        
        std::vector<Pixel> source = data;
        
        // Wavelength controls how tight the rings are
        float wavelength = 20.0f; 
        // Amplitude controls how much pixels move
        float amplitude = params.intensity / 5.0f;

        for (int y = region.y; y < region.y + region.height; ++y) {
            for (int x = region.x; x < region.x + region.width; ++x) {
                
                int dx = x - params.centerX;
                int dy = y - params.centerY;
                float dist = std::sqrt(dx*dx + dy*dy);
                
                if (dist > params.radius) continue;

                // Math: Offset based on Sine of distance
                float amount = std::sin(dist / wavelength) * amplitude;
                
                // Displacement vector (towards/away from center)
                // Normalize direction (dx/dist, dy/dist) and scale by amount
                float srcX = x + (dx / (dist + 0.1f)) * amount;
                float srcY = y + (dy / (dist + 0.1f)) * amount;

                // Sampling
                int sx = static_cast<int>(srcX);
                int sy = static_cast<int>(srcY);

                // Boundary clamp
                sx = std::max(0, std::min(imgWidth - 1, sx));
                sy = std::max(0, std::min(imgHeight - 1, sy));

                data[y * imgWidth + x] = source[sy * imgWidth + sx];
            }
        }
    }
};