#pragma once
#include <memory>
#include "IEffect.h"

// Include all concrete effect implementations
#include "Effects/InvertEffect.h"
#include "Effects/PixelSortEffect.h"
#include "Effects/ChromaticEffect.h"
#include "Effects/SwirlEffect.h"
#include "Effects/MosaicEffect.h"
#include "Effects/JitterEffect.h"
#include "Effects/ScanlineEffect.h"
#include "Effects/SobelEffect.h"
#include "Effects/RippleEffect.h"
#include "Effects/SolarizeEffect.h"
#include "Effects/RGBNoiseEffect.h"

/**
 * @enum EffectType
 * @brief Enumeration of all available visual effects.
 * Must match the enum definition in JavaScript bindings.
 */
enum class EffectType {
    NONE = 0,
    INVERT = 1,
    PIXEL_SORT = 2,
    CHROMATIC = 3,
    SWIRL = 4,
    MOSAIC = 5,
    JITTER = 6,
    SCANLINE = 7,
    SOBEL = 8,
    RIPPLE = 9,
    SOLARIZE = 10,
    RGB_NOISE = 11
};

/**
 * @class EffectFactory
 * @brief Implements the Factory Method pattern to create effect instances.
 */
class EffectFactory {
public:
    /**
     * @brief Creates and returns a unique pointer to an IEffect implementation.
     * @param type The EffectType enum identifier.
     * @return std::unique_ptr<IEffect> Pointer to the concrete effect or nullptr.
     */
    static std::unique_ptr<IEffect> createEffect(EffectType type) {
        switch (type) {
            case EffectType::INVERT:    return std::make_unique<InvertEffect>();
            case EffectType::PIXEL_SORT:return std::make_unique<PixelSortEffect>();
            case EffectType::CHROMATIC: return std::make_unique<ChromaticEffect>();
            case EffectType::SWIRL:     return std::make_unique<SwirlEffect>();
            case EffectType::MOSAIC:    return std::make_unique<MosaicEffect>();
            case EffectType::JITTER:    return std::make_unique<JitterEffect>();
            case EffectType::SCANLINE:  return std::make_unique<ScanlineEffect>();
            case EffectType::SOBEL:     return std::make_unique<SobelEffect>();
            case EffectType::RIPPLE:    return std::make_unique<RippleEffect>();
            case EffectType::SOLARIZE:  return std::make_unique<SolarizeEffect>();
            case EffectType::RGB_NOISE: return std::make_unique<RGBNoiseEffect>();
            case EffectType::NONE:
            default:
                return nullptr;
        }
    }
};