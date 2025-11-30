#pragma once
#include "Common.h"
#include <vector>

/**
 * @interface IEffect
 * @brief Interface (Strategy Pattern) that all glitch effects must implement.
 */
class IEffect {
public:
    virtual ~IEffect() = default;

    /**
     * @brief Applies the effect to the buffer within the specified region.
     * * @param data The entire image buffer (read/write).
     * @param imgWidth Total width of the image.
     * @param imgHeight Total height of the image.
     * @param region The bounding box to process (optimization).
     * @param params Configuration parameters (intensity, mask, etc).
     */
    virtual void apply(std::vector<Pixel>& data, int imgWidth, int imgHeight, 
                       const Region& region, const EffectParams& params) = 0;

protected:
    // Helper to check if a pixel is inside the circular bubble
    bool isInsideBubble(int x, int y, const EffectParams& params) {
        if (!params.useCircleMask) return true; // Full image mode
        
        int dx = x - params.centerX;
        int dy = y - params.centerY;
        return (dx * dx + dy * dy) <= (params.radius * params.radius);
    }
};