import { useState, useEffect } from 'react';
// We import the JS file, but TypeScript will now use the .d.ts definition we created above.
import createGlitchModule from './glitch_engine.js'; 
import type { GlitchEngine, WasmModule } from '../types/glitch-engine';

/**
 * @interface GlitchHookState
 * @brief Return type for the useGlitchEngine hook.
 */
interface GlitchHookState {
    engine: GlitchEngine | null;
    wasmModule: WasmModule | null;
    isLoaded: boolean;
}

/**
 * @hook useGlitchEngine
 * @brief Manages the lifecycle and initialization of the C++ WebAssembly engine.
 * @returns {GlitchHookState} The engine instance, the wasm module, and loading state.
 */
export const useGlitchEngine = (): GlitchHookState => {
    const [engine, setEngine] = useState<GlitchEngine | null>(null);
    const [wasmModule, setWasmModule] = useState<WasmModule | null>(null);
    const [isLoaded, setIsLoaded] = useState<boolean>(false);

    useEffect(() => {
        /**
         * @function initEngine
         * @brief Asynchronously loads and instantiates the Wasm module.
         */
        const initEngine = async (): Promise<void> => {
            try {
                // The import 'createGlitchModule' is strictly typed as GlitchModuleFactory now.
                const Module = await createGlitchModule({
                    locateFile: (file: string): string => `/${file}`, 
                });

                console.log("✅ C++ Engine Initialized Successfully", Module);
                
                // Polyfill HEAPU8 if missing (common in some Emscripten builds)
                if (!Module.HEAPU8) {
                    console.warn("⚠️ HEAPU8 missing on Module, attempting to polyfill...");
                    if (Module.wasmMemory) {
                        console.log("Found Module.wasmMemory");
                        Module.HEAPU8 = new Uint8Array(Module.wasmMemory.buffer);
                    } else if (Module.buffer) {
                        console.log("Found Module.buffer");
                        Module.HEAPU8 = new Uint8Array(Module.buffer);
                    } else {
                        console.error("❌ Could not find WebAssembly memory buffer!");
                    }
                }

                setWasmModule(Module);
                setEngine(new Module.GlitchEngine());
                setIsLoaded(true);
            } catch (error) {
                console.error("❌ Failed to load Wasm Module:", error);
            }
        };

        initEngine();

        // Cleanup function
        return () => {
            if (engine) {
                try {
                    engine.delete();
                } catch (e) {
                    console.warn("Engine cleanup failed (already deleted or invalid):", e);
                }
            }
        };
        // eslint-disable-next-line react-hooks/exhaustive-deps
    }, []);

    return { engine, wasmModule, isLoaded };
};