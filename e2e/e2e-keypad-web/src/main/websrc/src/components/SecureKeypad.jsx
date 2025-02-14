import React from 'react';
import '../style/keypad.css';

export default function SecureKeypad({ keypad, onKeyPressed, userInput }) {
    if (!keypad || !keypad.keypadImage || !keypad.hashList) {
        return <div>Loading keypad...</div>;
    }

    const buttonValues = [...Array(10).keys(), '', ''];

    return (
        <div className="keypad-wrapper">
            <div className="keypad-container">
                <img 
                    src={`data:image/png;base64,${keypad.keypadImage}`} 
                    alt="Secure Keypad" 
                    className="keypad-image"
                />
                <div className="keypad-overlay">
                    {keypad.hashList.map((hash, index) => (
                        <button 
                            key={index} 
                            className="keypad-button"
                            onClick={() => onKeyPressed(hash, buttonValues[index])}
                        >
                            <div className="button-overlay"></div>
                        </button>
                    ))}
                </div>
            </div>
        </div>
    );
}