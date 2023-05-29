import React, { useState, useEffect } from 'react';
import Invader from './Invader.jsx';
import Shooter from './Shooter.jsx';
import Score from './Score.jsx';
import Lives from './Lives.jsx';
import '../Styles/GameBoard.css'; // Import your CSS file

function GameBoard() {
    const [numInvaders, setNumInvaders] = useState(10); // Start with 10 invaders
    const [invaderPositions, setInvaderPositions] = useState(Array.from({ length: 10 }, (_, i) => ({x: i, y: 0}))); // Initial positions
    const [shooterPosition, setShooterPosition] = useState({x: 5, y: 9}); // Initial shooter position at bottom middle of the grid
    const [score, setScore] = useState(0); // Initial score
    const [lives, setLives] = useState(10); // Initial number of lives
    const [gameOver, setGameOver] = useState(false); // Game over state

    const shooterStyle = {
        gridColumn: shooterPosition.x + 1, // CSS grid columns start at 1
        gridRow: shooterPosition.y + 1     // CSS grid rows start at 1
    };

    // Move shooter based on user input
    useEffect(() => {
        const handleKeyPress = (event) => {
            switch (event.key) {
                case 'ArrowLeft':
                    setShooterPosition(prev => prev.x > 0 ? {x: prev.x - 1, y: prev.y} : prev);
                    break;
                case 'ArrowRight':
                    setShooterPosition(prev => prev.x < 9 ? {x: prev.x + 1, y: prev.y} : prev);
                    break;
                // Add other keys if needed...
            }
        };
        window.addEventListener('keydown', handleKeyPress);
        return () => {
            window.removeEventListener('keydown', handleKeyPress);
        };
    }, []);

    // Move invaders over time
    useEffect(() => {
        const intervalId = setInterval(() => {
            setInvaderPositions(prev =>
                prev.map(invader => ({
                    x: invader.x,
                    y: (invader.y + 1) % 10 // Simple movement, replace with your own logic
                }))
            );
        }, 1000);
        return () => {
            clearInterval(intervalId);
        };
    }, []);

    // Check for game over
    useEffect(() => {
        if (lives <= 0) {
            // End game - shooter lost
            setGameOver(true);
        }
        if (numInvaders <= 0) {
            // End game - shooter won
            setGameOver(true);
        }

        // Check for collisions with the shooter
        if (invaderPositions.some(invader => invader.x === shooterPosition.x && invader.y === shooterPosition.y)) {
            setLives(lives => lives - 1);
        }

    }, [lives, numInvaders, invaderPositions, shooterPosition]);

    return (
        <div className="game-board">
            <Score currentScore={score} />
            <Lives remainingLives={lives} />
            <div className="game-field">
                {invaderPositions.map((pos, index) => <Invader key={index} position={pos} />)}
                <Shooter position={shooterPosition} style={shooterStyle} />
            </div>
            {gameOver && <div className="game-over">Game Over</div>}
        </div>
    );
}

export default GameBoard;
