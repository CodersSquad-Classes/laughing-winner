import React, { useState } from 'react';

function Welcome({ onPlay }) {
    const [invaderCount, setInvaderCount] = useState('');

    const handleInputChange = (e) => {
        setInvaderCount(e.target.value);
    };

    const handlePlay = () => {
        const count = parseInt(invaderCount, 10);
        if (count >= 1 && count <= 19) {
            onPlay(count);
        } else {
            alert('Please enter a value between 1 and 19.');
        }
    };

    return (
        <div className="welcome">
            <h1>Welcome to the Game!</h1>
            <label htmlFor="invader-count">Number of Invaders:</label>
            <input
                type="number"
                id="invader-count"
                value={invaderCount}
                onChange={handleInputChange}
                min="1"
                max="20"
            />
            <button onClick={handlePlay}>Play</button>
        </div>
    );
}

export default Welcome;
