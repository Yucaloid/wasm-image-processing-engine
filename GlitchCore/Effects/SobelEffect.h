#pragma once
#include "../IEffect.h"
#include <cmath>
#include <algorithm>

/**
 * @class SobelEffect
 * @brief Performs edge detection using the Sobel operator.
 * Highlights high-contrast transitions (edges) and darkens flat areas.
 */
class SobelEffect : public IEffect {
public:
    void apply(std::vector<Pixel>& data, int imgWidth, int imgHeight, 
               const Region& region, const EffectParams& params) override {
        
        // Copy source to read neighbors safely
        std::vector<Pixel> source = data;
        
        // Sobel Kernels for X and Y directions
        int gx[3][3] = { {-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1} };
        int gy[3][3] = { {-1, -2, -1}, {0, 0, 0}, {1, 2, 1} };

        for (int y = region.y; y < region.y + region.height; ++y) {
            for (int x = region.x; x < region.x + region.width; ++x) {
                
                if (!isInsideBubble(x, y, params)) continue;

                float sumX = 0;
                float sumY = 0;

                // Convolution Loop (3x3 kernel)
                for (int ky = -1; ky <= 1; ky++) {
                    for (int kx = -1; kx <= 1; kx++) {
                        int pX = std::min(std::max(x + kx, 0), imgWidth - 1);
                        int pY = std::min(std::max(y + ky, 0), imgHeight - 1);
                        
                        int idx = (pY * imgWidth) + pX;
                        // Use luminance for edge calculation
                        float val = source[idx].getLuminance();

                        sumX += val * gx[ky + 1][kx + 1];
                        sumY += val * gy[ky + 1][kx + 1];
                    }
                }

                // Gradient Magnitude
                int magnitude = static_cast<int>(std::sqrt(sumX * sumX + sumY * sumY));
                
                // Clamp and invert capability based on intensity
                // If intensity is high, edges are neon colored, background black.
                uint8_t edgeVal = static_cast<uint8_t>(std::min(255, magnitude));
                
                // Art style: Green edges (Matrix style) or White edges
                int idx = (y * imgWidth) + x;
                
                if (params.intensity > 50) {
                    // Neon Mode
                    data[idx] = {0, edgeVal, 0, 255}; 
                } else {
                    // Standard B&W Edges
                    data[idx] = {edgeVal, edgeVal, edgeVal, 255};
                }
            }
        }
    }
};