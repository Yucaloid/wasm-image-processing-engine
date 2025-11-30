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
        <div style={{ marginBottom: '15px' }}>
            <div style={{ display: 'flex', justifyContent: 'space-between', color: '#ccc', fontSize: '0.9rem' }}>
                <label>{label}</label>
                <span>{value}</span>
            </div>
            <input
                type="range"
                min={min}
                max={max}
                value={value}
                onChange={(e) => onChange(Number(e.target.value))}
                style={{ width: '100%', accentColor: '#007bff' }}
            />
        </div>
    );
};