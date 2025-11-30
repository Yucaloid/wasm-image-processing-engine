#include <vector>
#include <cstring> // for std::memcpy
#include <algorithm>
#include "Common.h"
#include "EffectFactory.h"

class GlitchEngine {
private:
    std::vector<Pixel> originalBuffer; // The clean backup
    std::vector<Pixel> displayBuffer;  // The dirty render
    int width = 0;
    int height = 0;

public:
    GlitchEngine() {}

    // 1. Memory Setup
    void loadBox(int w, int h) {
        width = w;
        height = h;
        originalBuffer.resize(w * h);
        displayBuffer.resize(w * h);
    }

    // 2. Accessors for JS
    uintptr_t getOriginalPointer() { return reinterpret_cast<uintptr_t>(originalBuffer.data()); }
    uintptr_t getDisplayPointer() { return reinterpret_cast<uintptr_t>(displayBuffer.data()); }

    /**
     * @brief The Main Render Loop.
     * 1. Resets the frame (Healing).
     * 2. Gets the correct effect from Factory.
     * 3. Applies the effect.
     */
    void renderFrame(int mouseX, int mouseY, int radius, int effectId, float intensity) {
        // Step A: "Heal" the frame (Copy Original -> Display)
        // This ensures the glitch doesn't paint permanently over the image
        if (!originalBuffer.empty()) {
             std::memcpy(displayBuffer.data(), originalBuffer.data(), originalBuffer.size() * sizeof(Pixel));
        }

        // Step B: Get Strategy
        EffectType type = static_cast<EffectType>(effectId);
        auto effect = EffectFactory::createEffect(type);

        if (!effect) return; // NONE or Invalid

        // Step C: Prepare Context
        // Optimization: Define a Bounding Box around the mouse to avoid scanning the whole HD image
        Region region;
        region.x = std::max(0, mouseX - radius);
        region.y = std::max(0, mouseY - radius);
        
        // Clamp width/height to image boundaries
        int endX = std::min(width, mouseX + radius);
        int endY = std::min(height, mouseY + radius);
        
        region.width = endX - region.x;
        region.height = endY - region.y;

        EffectParams params;
        params.intensity = intensity;
        params.useCircleMask = true; // Always bubble mode for interaction
        params.centerX = mouseX;
        params.centerY = mouseY;
        params.radius = radius;

        // Step D: Execute
        effect->apply(displayBuffer, width, height, region, params);
    }
};