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
            className={`effect-button ${isActive ? 'active' : ''}`}
        >
            {label}
        </button>
    );
};