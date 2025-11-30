import { useState, useEffect } from 'react';
// We import the JS file, but TypeScript will now use the .d.ts definition we created above.
import createGlitchModule from './glitch_engine.js'; 
import type { GlitchEngine, WasmModule, GlitchModuleFactory } from '../types/glitch-engine';

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
                const factory = createGlitchModule as unknown as GlitchModuleFactory;

                const Module = await factory({
                    // CRITICAL FIX FOR GITHUB PAGES:
                    // Use Vite's BASE_URL to correctly locate the .wasm file in both
                    // localhost ('/') and production ('/wasm-image-processing-engine/')
                    locateFile: (file: string): string => {
                        const baseURL = import.meta.env.BASE_URL;
                        // Ensure we don't have double slashes if baseURL ends with /
                        const prefix = baseURL.endsWith('/') ? baseURL : `${baseURL}/`;
                        return `${prefix}${file}`;
                    }, 
                });
                console.log("✅ C++ Engine Initialized Successfully");
                
                // Polyfill HEAPU8 if missing (common in some Emscripten builds)
                if (!Module.HEAPU8) {
                    console.warn("⚠️ HEAPU8 missing on Module, attempting to polyfill...");
                    if (Module.wasmMemory) {
                         // Type assertion to access distinct property
                        Module.HEAPU8 = new Uint8Array(Module.wasmMemory.buffer);
                    } else if (Module.buffer) {
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