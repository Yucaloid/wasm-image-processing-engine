#pragma once
#include "../IEffect.h"
#include <cmath>
#include <vector>

/**
 * @class SwirlEffect
 * @brief Applies a twisting distortion to the image within the bubble.
 * Uses trigonometric rotation based on the distance from the center.
 */
class SwirlEffect : public IEffect {
public:
    /**
     * @brief Applies the swirl algorithm.
     * Logic: Calculates a rotation angle theta that increases as the pixel gets closer to the center.
     * @param data The image buffer.
     * @param imgWidth Image width.
     * @param imgHeight Image height.
     * @param region The bounding box optimization.
     * @param params Effect parameters (intensity controls the max rotation angle).
     */
    void apply(std::vector<Pixel>& data, int imgWidth, int imgHeight, 
               const Region& region, const EffectParams& params) override {
        
        // Create a temporary copy of the buffer to read original coordinates.
        // This is crucial because the transformation is non-linear and reading 
        // from the already-modified buffer would create visual artifacts.
        std::vector<Pixel> source = data;
        
        // Scale intensity to a reasonable radian angle (e.g., intensity 100 = ~10 radians)
        float angleParam = params.intensity / 10.0f; 

        for (int y = region.y; y < region.y + region.height; ++y) {
            for (int x = region.x; x < region.x + region.width; ++x) {
                
                // --- Bubble Geometry Calculations ---
                int dx = x - params.centerX;
                int dy = y - params.centerY;
                float dist = std::sqrt(dx*dx + dy*dy);
                
                // Optimization: Skip pixels outside the radius
                if (dist > params.radius) continue;

                // 1. Calculate rotation angle theta
                // The angle is strongest at the center (dist=0) and 0 at the edge (dist=radius).
                float percent = (params.radius - dist) / params.radius;
                float theta = percent * percent * angleParam;
                
                // 2. 2D Rotation Matrix application
                float sinTheta = std::sin(theta);
                float cosTheta = std::cos(theta);
                
                // Calculate the source coordinate (inverse mapping) relative to the center
                float srcX = params.centerX + (dx * cosTheta - dy * sinTheta);
                float srcY = params.centerY + (dx * sinTheta + dy * cosTheta);

                // 3. Sampling with Boundary Checks
                int sx = static_cast<int>(srcX);
                int sy = static_cast<int>(srcY);

                if (sx >= 0 && sx < imgWidth && sy >= 0 && sy < imgHeight) {
                    data[y * imgWidth + x] = source[sy * imgWidth + sx];
                }
            }
        }
    }
};