import React, { useState, useEffect, useRef } from 'react';
import Invader from './Invader.jsx';
import Shooter from './Shooter.jsx';
import Score from './Score.jsx';
import Lives from './Lives.jsx';
import GameOver from './GameOver.jsx';
import '../Styles/GameBoard.css';
import '../Styles/Shooter.css';
import '../Styles/Invader.css';

function GameBoard({ numInvaders }) {
    const [invaderPositions, setInvaderPositions] = useState(
        Array.from({ length: numInvaders }, (_, i) => ({ x: i, y: 0 }))
    );
    const [shooterPositionX, setShooterPositionX] = useState(5);
    const [score, setScore] = useState(0);
    const [lives, setLives] = useState(10);
    const [gameOver, setGameOver] = useState(false);
    const [shooting, setShooting] = useState(false); // New state variable for shooting animation
    const [invaderAboutToShoot, setInvaderAboutToShoot] = useState(null);
    const [invadersLeft, setInvadersLeft] = useState(numInvaders)
    const socketRef = useRef();

    useEffect(() => {
        socketRef.current = new WebSocket('ws://localhost:9000');

        socketRef.current.addEventListener('open', function (event) {
            socketRef.current.send('start ' + numInvaders);
        });

        socketRef.current.addEventListener('message', function (event) {
            console.log('Message from server: ', event.data);
            const gameState = JSON.parse(event.data);
            setInvaderPositions(gameState.invaderPositions);
            setShooterPositionX(gameState.shooterPosition);
            setScore(gameState.score);
            setLives(gameState.lives);
            setInvaderAboutToShoot(gameState.invaderAboutToShoot);
            setInvadersLeft(gameState.numInvaders);
        });

        return () => {
            socketRef.current.close();
        };
    }, [numInvaders]);

    const shooterStyle = {
        gridColumn: shooterPositionX + 1,
        gridRow: 10,
    };

    useEffect(() => {
        const handleKeyPress = (event) => {
            switch (event.key) {
                case 'ArrowLeft':
                    console.log('Sending left command to server');
                    socketRef.current.send('left');
                    break;
                case 'ArrowRight':
                    console.log('Sending right command to server');
                    socketRef.current.send('right');
                    break;
                case ' ':
                    console.log('Sending shoot command to server');
                    if (!shooting) {
                        socketRef.current.send('shoot');
                        setShooting(true);
                        setTimeout(() => {
                            setShooting(false);
                        }, 300); // Adjust the duration of the shooting animation as needed
                    }
                    break;
            }
        };

        window.addEventListener('keydown', handleKeyPress);

        return () => {
            window.removeEventListener('keydown', handleKeyPress);
        };
    }, [shooting]);

    useEffect(() => {
        if (lives <= 0 || invadersLeft <= 0) {
            console.log("GAME OVER")
            setGameOver(true);
        }
    }, [lives, invadersLeft]);

    return (
        <div className="game-board">
            <Score currentScore={score} />
            <Lives remainingLives={lives} />
            <div className="game-field">
                {invaderPositions.map((pos, index) => (
                    <Invader key={pos.id} position={pos} aboutToShoot={pos.id === invaderAboutToShoot} />
                ))}
                <Shooter style={shooterStyle} shooting={shooting} /> {/* Pass the shooting state to the Shooter component */}
            </div>
            {gameOver && <GameOver score={score} />}
        </div>
    );
}

export default GameBoard;
