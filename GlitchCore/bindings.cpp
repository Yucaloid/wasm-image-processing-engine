#include <emscripten/bind.h>
// Include the main engine logic (Unity build approach)
#include "GlitchEngine.cpp" 

using namespace emscripten;

/**
 * @brief EMSCRIPTEN_BINDINGS defines the interface between C++ and JavaScript.
 */
EMSCRIPTEN_BINDINGS(glitch_module) {
    
    // Bind the EffectType enum so JavaScript can refer to effects by name/value
    enum_<EffectType>("EffectType")
        .value("NONE", EffectType::NONE)
        .value("INVERT", EffectType::INVERT)
        .value("PIXEL_SORT", EffectType::PIXEL_SORT)
        .value("CHROMATIC", EffectType::CHROMATIC)
        .value("SWIRL", EffectType::SWIRL)
        .value("MOSAIC", EffectType::MOSAIC)
        .value("JITTER", EffectType::JITTER)
        .value("SCANLINE", EffectType::SCANLINE)
        .value("SOBEL", EffectType::SOBEL)
        .value("RIPPLE", EffectType::RIPPLE)
        .value("SOLARIZE", EffectType::SOLARIZE)
        .value("RGB_NOISE", EffectType::RGB_NOISE);

    // Bind the main Engine class
    class_<GlitchEngine>("GlitchEngine")
        .constructor<>()
        .function("loadBox", &GlitchEngine::loadBox)
        .function("getOriginalPointer", &GlitchEngine::getOriginalPointer)
        .function("getDisplayPointer", &GlitchEngine::getDisplayPointer)
        .function("renderFrame", &GlitchEngine::renderFrame);
}