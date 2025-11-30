#pragma once
#include <cstdint>
#include <cmath>
#include <vector>

/**
 * @brief Represents a single RGBA pixel.
 */
struct Pixel {
    uint8_t r, g, b, a;

    // Helper: Luminance for sorting algorithms
    float getLuminance() const {
        return (0.299f * r) + (0.587f * g) + (0.114f * b);
    }
};

/**
 * @brief Defines a rectangular area of interest to apply an effect.
 * Used for optimization: we only process pixels within this box.
 */
struct Region {
    int x, y;       // Top-left corner
    int width, height;
};

/**
 * @brief Context parameters passed to every effect.
 * Allows extending functionality without changing method signatures.
 */
struct EffectParams {
    float intensity; // 0.0 to 1.0 (or higher)
    bool useCircleMask; // If true, applies circular mask logic
    int centerX;    // For bubble effect
    int centerY;    // For bubble effect
    int radius;     // For bubble effect
};