import React from 'react';

interface EffectButtonProps {
    label: string;
    isActive: boolean;
    onClick: () => void;
}

export const EffectButton: React.FC<EffectButtonProps> = ({ label, isActive, onClick }) => {
    return (
        <button
            onClick={onClick}
            style={{
                padding: '8px 12px',
                margin: '4px',
                backgroundColor: isActive ? '#007bff' : '#2a2a2a',
                color: 'white',
                border: '1px solid #444',
                borderRadius: '4px',
                cursor: 'pointer',
                transition: 'all 0.2s ease',
                fontWeight: isActive ? 'bold' : 'normal'
            }}
        >
            {label}
        </button>
    );
};