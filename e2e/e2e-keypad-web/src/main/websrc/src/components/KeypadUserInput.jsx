import React from 'react';
import '../style/keypad.css'

export default function KeypadUserInput({ userInput }) {
    return (
        <div className="user-input-display">
            {Array(6).fill(0).map((_, index) => (
                <span 
                    key={index} 
                    className={`input-dot ${index < userInput.length ? 'filled' : ''}`}
                />
            ))}
        </div>
    );
}