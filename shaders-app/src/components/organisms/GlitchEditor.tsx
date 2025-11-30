import React, { useRef, useState, useEffect, useCallback, type ChangeEvent, type MouseEvent } from 'react';
import { useGlitchEngine } from '../../utils/useGlitchEngine';
import { EffectButton } from '../atoms/EffectButton';
import { RangeSlider } from '../atoms/RangeSlider';
import './GlitchEditor.css';

/**
 * @enum EffectType
 * @brief Enumeration of available visual effects.
 * MUST match the C++ enum definition.
 */
const EffectType = {
    NONE: 0,
    INVERT: 1,
    PIXEL_SORT: 2,
    CHROMATIC: 3,
    SWIRL: 4,
    MOSAIC: 5,
    JITTER: 6,
    SCANLINE: 7,
    SOBEL: 8,
    RIPPLE: 9,
    SOLARIZE: 10,
    RGB_NOISE: 11
} as const;

type EffectType = typeof EffectType[keyof typeof EffectType];

/**
 * @component GlitchEditor
 * @brief Main container component for the image processing tool.
 * Handles user interactions, canvas rendering, and C++ engine communication.
 */
export const GlitchEditor: React.FC = () => {
    const { engine, wasmModule, isLoaded } = useGlitchEngine();
    const canvasRef = useRef<HTMLCanvasElement>(null);
    const [imageUploaded, setImageUploaded] = useState<boolean>(false);
    
    // UI State
    const [activeEffect, setActiveEffect] = useState<EffectType>(EffectType.PIXEL_SORT);
    const [radius, setRadius] = useState<number>(150);
    const [intensity, setIntensity] = useState<number>(50);
    const [editMode, setEditMode] = useState<'bubble' | 'full'>('bubble');

    /**
     * @function renderToCanvas
     * @brief Helper to paint the C++ buffer back to the canvas.
     */
    const renderToCanvas = useCallback(() => {
        const canvas = canvasRef.current;
        if (!canvas || !engine || !wasmModule) return;

        const ctx = canvas.getContext('2d');
        if (!ctx) return;
        
        const displayPtr = engine.getDisplayPointer();
        
        const wasmView = new Uint8ClampedArray(
            wasmModule.HEAPU8.buffer as ArrayBuffer, 
            displayPtr, 
            canvas.width * canvas.height * 4
        );
        
        const newImageData = new ImageData(wasmView, canvas.width, canvas.height);
        ctx.putImageData(newImageData, 0, 0);
    }, [engine, wasmModule]);

    /**
     * @function applyFullImageEffect
     * @brief Applies the effect to the entire image by using a large radius.
     */
    const applyFullImageEffect = useCallback(() => {
        if (!engine || !canvasRef.current) return;
        const canvas = canvasRef.current;
        
        // Center of the image
        const cx = canvas.width / 2;
        const cy = canvas.height / 2;
        
        // Radius large enough to cover corners
        const maxDim = Math.max(canvas.width, canvas.height);
        const fullRadius = maxDim * 1.5;

        engine.renderFrame(cx, cy, fullRadius, activeEffect, intensity);
        renderToCanvas();
    }, [engine, activeEffect, intensity, renderToCanvas]);

    /**
     * @function handleImageUpload
     * @brief Processes the uploaded image file and syncs it with C++ memory.
     * @param e The change event from the file input.
     */
    const handleImageUpload = (e: ChangeEvent<HTMLInputElement>): void => {
        const file = e.target.files?.[0];
        if (!file || !engine || !wasmModule) return;

        const reader = new FileReader();
        
        reader.onload = (evt: ProgressEvent<FileReader>) => {
            const img = new Image();
            
            img.onload = () => {
                const canvas = canvasRef.current;
                if (!canvas) return;

                // Set canvas dimensions to match image
                canvas.width = img.width;
                canvas.height = img.height;
                
                const ctx = canvas.getContext('2d', { willReadFrequently: true });
                if (!ctx) return;

                ctx.drawImage(img, 0, 0);

                // --- C++ Memory Sync ---
                engine.loadBox(img.width, img.height);
                
                const imageData = ctx.getImageData(0, 0, img.width, img.height);
                const originalPtr = engine.getOriginalPointer();
                
                // Create a typed view into the Wasm heap
                const wasmHeap = new Uint8Array(wasmModule.HEAPU8.buffer, originalPtr, img.width * img.height * 4);
                wasmHeap.set(imageData.data);

                // Initial render to clear artifacts
                engine.renderFrame(-1000, -1000, 0, 0, 0); 
                setImageUploaded(true);

                // If in full mode, apply effect immediately
                if (editMode === 'full') {
                    setTimeout(applyFullImageEffect, 100);
                }
            };

            if (typeof evt.target?.result === 'string') {
                img.src = evt.target.result;
            }
        };
        
        reader.readAsDataURL(file);
    };

    // Re-run full effect when parameters change in 'full' mode
    useEffect(() => {
        if (editMode === 'full' && imageUploaded) {
            applyFullImageEffect();
        }
    }, [editMode, imageUploaded, applyFullImageEffect]);

    /**
     * @function handleMouseMove
     * @brief Triggers the C++ render loop based on mouse position (Bubble Mode only).
     * @param e The mouse event from the canvas.
     */
    const handleMouseMove = (e: MouseEvent<HTMLCanvasElement>): void => {
        if (!imageUploaded || !engine || !wasmModule || !canvasRef.current) return;
        if (editMode === 'full') return; // Disable mouse interaction in full mode

        const canvas = canvasRef.current;
        const rect = canvas.getBoundingClientRect();
        
        // Calculate scaling factors for CSS resized canvas
        const scaleX = canvas.width / rect.width;
        const scaleY = canvas.height / rect.height;
        
        const x = (e.clientX - rect.left) * scaleX;
        const y = (e.clientY - rect.top) * scaleY;

        // Execute C++ Logic
        engine.renderFrame(x, y, radius, activeEffect, intensity);
        renderToCanvas();
    };

    /**
     * @function handleDownload
     * @brief Downloads the current canvas content as a PNG.
     */
    const handleDownload = () => {
        const canvas = canvasRef.current;
        if (!canvas) return;

        const link = document.createElement('a');
        link.download = `glitch-art-${Date.now()}.png`;
        link.href = canvas.toDataURL('image/png');
        link.click();
    };

    if (!isLoaded) return <div className="loading-screen">Loading Wasm Core...</div>;

    return (
        <div className="glitch-editor-container">
            <aside className="editor-sidebar">
                <header className="sidebar-header">
                    <span>⚡</span> GlitchCore
                </header>
                
                <section className="sidebar-section">
                    <div className="section-title">Source</div>
                    <div className="file-input-wrapper">
                        <input 
                            type="file" 
                            accept="image/*" 
                            onChange={handleImageUpload} 
                            className="file-input"
                        />
                    </div>
                </section>

                <section className="sidebar-section">
                    <div className="section-title">Mode</div>
                    <div className="mode-toggle">
                        <button 
                            className={`mode-btn ${editMode === 'bubble' ? 'active' : ''}`}
                            onClick={() => setEditMode('bubble')}
                        >
                            Bubble
                        </button>
                        <button 
                            className={`mode-btn ${editMode === 'full' ? 'active' : ''}`}
                            onClick={() => setEditMode('full')}
                        >
                            Full Image
                        </button>
                    </div>
                </section>

                <section className="sidebar-section">
                    <div className="section-title">Effects</div>
                    <div className="effects-grid">
                        {(Object.keys(EffectType) as Array<keyof typeof EffectType>)
                            .filter(key => key !== 'NONE')
                            .map(key => (
                                <EffectButton 
                                    key={key} 
                                    label={key.toString()} 
                                    isActive={activeEffect === EffectType[key]} 
                                    onClick={() => setActiveEffect(EffectType[key])} 
                                />
                            ))
                        }
                    </div>
                </section>

                <section className="sidebar-section">
                    <div className="section-title">Parameters</div>
                    {editMode === 'bubble' && (
                        <RangeSlider label="Bubble Radius" value={radius} min={50} max={500} onChange={setRadius} />
                    )}
                    <RangeSlider label="Intensity" value={intensity} min={1} max={100} onChange={setIntensity} />
                </section>

                <section className="sidebar-section">
                    <button onClick={handleDownload} className="download-btn" disabled={!imageUploaded}>
                        Download Result ⬇️
                    </button>
                </section>
            </aside>

            <main className="editor-main">
                {!imageUploaded && (
                    <div className="empty-state">
                        <div className="empty-state-icon">🖼️</div>
                        <p>Upload an image to start glitching</p>
                    </div>
                )}
                <canvas 
                    ref={canvasRef} 
                    onMouseMove={handleMouseMove} 
                    className={`editor-canvas ${editMode === 'full' ? 'full-mode' : ''}`}
                    style={{ display: imageUploaded ? 'block' : 'none' }}
                />
            </main>
        </div>
    );
};