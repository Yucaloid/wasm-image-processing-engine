import React from 'react';

interface RangeSliderProps {
    label: string;
    value: number;
    min: number;
    max: number;
    onChange: (val: number) => void;
}

export const RangeSlider: React.FC<RangeSliderProps> = ({ label, value, min, max, onChange }) => {
    return (
        <div className="range-slider-container">
            <div className="range-slider-header">
                <label>{label}</label>
                <span>{value}</span>
            </div>
            <input
                type="range"
                min={min}
                max={max}
                value={value}
                onChange={(e) => onChange(Number(e.target.value))}
                className="range-slider-input"
            />
        </div>
    );
};