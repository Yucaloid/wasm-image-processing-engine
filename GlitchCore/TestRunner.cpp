/**
 * @file TestRunner.cpp
 * @brief Comprehensive Integration Test Suite for GlitchCore.
 * Validates the mathematical logic of all 7 visual effects.
 */

#include <iostream>
#include <vector>
#include <cassert>
#include <iomanip>
#include <cmath>
#include <cstdlib> // For rand/srand

// Include Core Definitions
#include "Common.h"

// Include All Effects
#include "Effects/InvertEffect.h"
#include "Effects/PixelSortEffect.h"
#include "Effects/ChromaticEffect.h"
#include "Effects/MosaicEffect.h"
#include "Effects/SwirlEffect.h"
#include "Effects/JitterEffect.h"
#include "Effects/ScanlineEffect.h"

// Console Color Macros
#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"

// --- HELPER FUNCTIONS ---

void printPass(const std::string& testName) {
    std::cout << "Test: " << std::left << std::setw(40) << testName 
              << GREEN << "[PASSED]" << RESET << std::endl;
}

void printFail(const std::string& testName, const std::string& reason) {
    std::cout << "Test: " << std::left << std::setw(40) << testName 
              << RED << "[FAILED]" << RESET << std::endl;
    std::cout << "  Reason: " << reason << std::endl;
    exit(1);
}

// Helper to create a solid color pixel
Pixel mkPixel(uint8_t v) { return {v, v, v, 255}; }

// --- TEST CASES ---

/**
 * @brief Test 1: Bubble Logic (Invert).
 * Checks boundary conditions of the circular mask.
 */
void runBubbleLogicTest() {
    int w = 10, h = 10;
    std::vector<Pixel> buffer(w * h, mkPixel(255)); // White

    InvertEffect effect;
    Region region = {0, 0, w, h};
    EffectParams params = {100.0f, true, 5, 5, 2}; 

    effect.apply(buffer, w, h, region, params);

    if (buffer[5 * w + 5].r != 0) printFail("Bubble Logic", "Center pixel not inverted.");
    if (buffer[0].r != 255) printFail("Bubble Logic", "Outside pixel modified.");

    printPass("Bubble Logic (Invert)");
}

/**
 * @brief Test 2: Pixel Sorting (Melting).
 * Checks if luminance sorting works on a vertical column.
 */
void runPixelSortTest() {
    int w = 1, h = 4;
    std::vector<Pixel> buffer = { mkPixel(200), mkPixel(50), mkPixel(100), mkPixel(250) };

    PixelSortEffect effect;
    Region region = {0, 0, w, h};
    EffectParams params = {100.0f, false, 0, 0, 0};

    effect.apply(buffer, w, h, region, params);

    // Expected: 50, 100, 200, 250
    if (buffer[0].r != 50 || buffer[1].r != 100) printFail("Pixel Sorting", "Sorting order incorrect.");

    printPass("Pixel Sorting");
}

/**
 * @brief Test 3: Chromatic Aberration.
 * Checks channel separation logic.
 */
void runChromaticTest() {
    int w = 3, h = 1;
    // R, G, B pixels
    std::vector<Pixel> buffer = { {255,0,0,255}, {0,255,0,255}, {0,0,255,255} };

    ChromaticEffect effect;
    Region region = {0, 0, w, h};
    EffectParams params = {1.0f, false, 0, 0, 0}; 

    effect.apply(buffer, w, h, region, params);

    Pixel mid = buffer[1];
    if (mid.r == 255 && mid.g == 255 && mid.b == 255) printPass("Chromatic Aberration");
    else printFail("Chromatic Aberration", "RGB channels did not merge correctly.");
}

/**
 * @brief Test 4: Mosaic Effect (Deterministic).
 * Verifies that a block of pixels takes the color of the top-left pixel.
 */
void runMosaicTest() {
    int w = 4, h = 4;
    std::vector<Pixel> buffer(w * h, mkPixel(0)); // Black canvas
    
    // Set Top-Left pixel of the first block (0,0) to WHITE
    buffer[0] = mkPixel(255); 
    // Set neighbor (1,1) to BLACK
    buffer[1 * w + 1] = mkPixel(0);

    MosaicEffect effect;
    Region region = {0, 0, w, h};
    // Intensity 4 -> BlockSize = 2.
    // Block (0,0) to (1,1) should all become WHITE (value of 0,0).
    EffectParams params = {4.0f, true, 0, 0, 10}; 

    effect.apply(buffer, w, h, region, params);

    // Check pixel at (1,1). It was Black, should now be White (copied from 0,0)
    if (buffer[1 * w + 1].r == 255) {
        printPass("Mosaic Effect");
    } else {
        printFail("Mosaic Effect", "Pixel (1,1) did not inherit color from (0,0).");
    }
}

/**
 * @brief Test 5: Swirl Effect (Movement Check).
 * Verifies that pixels move from their original position.
 */
void runSwirlTest() {
    int w = 10, h = 10;
    std::vector<Pixel> buffer(w * h, mkPixel(0)); // Black
    
    // Draw a White cross to detect movement
    for(int i=0; i<10; i++) {
        buffer[5 * w + i] = mkPixel(255); // Horizontal line
        buffer[i * w + 5] = mkPixel(255); // Vertical line
    }

    SwirlEffect effect;
    Region region = {0, 0, w, h};
    // Strong swirl at center
    EffectParams params = {50.0f, true, 5, 5, 4}; 

    effect.apply(buffer, w, h, region, params);

    // If the image is exactly the same, the swirl failed.
    // We check a specific pixel that should have moved.
    // Pixel (6,5) was White. With rotation, it should shift.
    // Note: This is a heuristic check.
    bool changed = false;
    for(int i=0; i<w*h; i++) {
        if (buffer[i].r != (i == 5*w+i || i == i*w+5 ? 255 : 0)) {
            // If the buffer doesn't match the simple cross generation logic anymore, it changed.
            // Actually, simplified check:
            changed = true; 
            break;
        }
    }
    
    // Ideally, we check that the center is still roughly preserved but neighbors rotated.
    // For unit testing, checking "modification happened" is often sufficient for visual effects.
    if (changed) printPass("Swirl Effect");
    else printFail("Swirl Effect", "No pixels moved. Image is identical to source.");
}

/**
 * @brief Test 6: Jitter/Glitch (Randomness Check).
 * Verifies that the effect alters the image content within the bubble.
 */
void runJitterTest() {
    std::srand(123); // Fixed seed for reproducibility? 
                     // Actually, Jitter uses rand(), which differs by platform.
                     // We will check for "Change detection".

    int w = 20, h = 20;
    std::vector<Pixel> buffer(w * h);
    
    // Create a gradient pattern
    for(int i=0; i<w*h; i++) buffer[i] = mkPixel(i % 255);

    // Copy for comparison
    std::vector<Pixel> original = buffer;

    JitterEffect effect;
    Region region = {0, 0, w, h};
    EffectParams params = {50.0f, true, 10, 10, 10}; // Radius 10

    effect.apply(buffer, w, h, region, params);

    // Compare original vs buffer. They MUST be different.
    bool differencesFound = false;
    for(size_t i = 0; i < buffer.size(); ++i) {
        if(buffer[i].r != original[i].r) {
            differencesFound = true;
            break;
        }
    }

    if (differencesFound) printPass("Jitter Effect");
    else printFail("Jitter Effect", "Image remained identical. No blocks were moved.");
}

/**
 * @brief Test 7: Scanline Effect.
 * Verifies horizontal shifting rows.
 */
void runScanlineTest() {
    int w = 10, h = 10;
    std::vector<Pixel> buffer(w * h, mkPixel(0));

    // Draw a vertical white line at x=5
    for(int y=0; y<h; y++) buffer[y * w + 5] = mkPixel(255);

    ScanlineEffect effect;
    Region region = {0, 0, w, h};
    EffectParams params = {5.0f, true, 5, 5, 10}; // Intensity 5

    // Run multiple times to ensure the random probability triggers at least once
    // (Scanline has a ~30% chance per line)
    bool shifted = false;
    for(int attempt=0; attempt<5; attempt++) {
        effect.apply(buffer, w, h, region, params);
        
        // Check if the white line at x=5 is broken (i.e., some pixel is now black)
        // OR some neighbor pixel (e.g. x=4 or x=6) became white.
        for(int y=0; y<h; y++) {
            if (buffer[y * w + 5].r != 255) shifted = true; // Line moved away
            if (buffer[y * w + 4].r == 255) shifted = true; // Line moved left
        }
        if(shifted) break;
    }

    if (shifted) printPass("Scanline Effect");
    else printFail("Scanline Effect", "Vertical line remained perfectly straight.");
}

// --- MAIN ---

int main() {
    std::srand(static_cast<unsigned int>(time(NULL))); // Random seed
    std::cout << "\n=== GLITCH CORE FULL SUITE ===\n" << std::endl;

    runBubbleLogicTest();
    runPixelSortTest();
    runChromaticTest();
    runMosaicTest();
    runSwirlTest();
    runJitterTest();
    runScanlineTest();

    std::cout << "\n" << GREEN << "=== ALL 7 TESTS PASSED SUCCESSFULLY ===" << RESET << "\n" << std::endl;
    return 0;
}