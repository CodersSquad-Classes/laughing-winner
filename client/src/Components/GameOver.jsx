import React from 'react';
import '../Styles/GameOver.css';

function GameOver({ score }) {
    return (
        <div className="game-over-popup">
            <h2>Game Over</h2>
            <p>Your score: {score}</p>
            <button onClick={() => window.location.reload(false)}>Try Again</button>
        </div>
    );
}

export default GameOver;
