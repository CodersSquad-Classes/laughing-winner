import React, { useState, useEffect, useRef } from 'react';
import Invader from './Invader.jsx';
import Shooter from './Shooter.jsx';
import Score from './Score.jsx';
import Lives from './Lives.jsx';
import '../Styles/GameBoard.css';

function GameBoard() {
    const [numInvaders, setNumInvaders] = useState(10);
    const [invaderPositions, setInvaderPositions] = useState(Array.from({ length: 10 }, (_, i) => ({x: i, y: 0})));
    const [shooterPositionX, setShooterPositionX] = useState(5);
    const [score, setScore] = useState(0);
    const [lives, setLives] = useState(10);
    const [gameOver, setGameOver] = useState(false);

    const socketRef = useRef();

    useEffect(() => {
        socketRef.current = new WebSocket('ws://localhost:9000');

        socketRef.current.addEventListener('open', function (event) {
            socketRef.current.send('start');
        });

        socketRef.current.addEventListener('message', function (event) {
            console.log('Message from server: ', event.data);
            const gameState = JSON.parse(event.data);
            setNumInvaders(gameState.numInvaders);
            setInvaderPositions(gameState.invaderPositions);
            setShooterPositionX(gameState.shooterPosition);
            setScore(gameState.score);
            setLives(gameState.lives);
        });

        return () => {
            socketRef.current.close();
        };
    }, []);

    const shooterStyle = {
        gridColumn: shooterPositionX + 1,
        gridRow: 10
    };

    useEffect(() => {
        const handleKeyPress = (event) => {
            switch (event.key) {
                case 'ArrowLeft':
                    console.log("Sending left command to server");
                    socketRef.current.send('left');
                    break;
                case 'ArrowRight':
                    console.log("Sending right command to server");
                    socketRef.current.send('right');
                    break;
                case ' ':
                    console.log("Sending shoot command to server");
                    socketRef.current.send('shoot');
                    break;
            }
        };

        window.addEventListener('keydown', handleKeyPress);

        return () => {
            window.removeEventListener('keydown', handleKeyPress);
        };
    }, []);

    useEffect(() => {
        if (lives <= 0) {
            setGameOver(true);
        }
        if (numInvaders <= 0) {
            setGameOver(true);
        }
    }, [lives, numInvaders]);

    return (
        <div className="game-board">
            <Score currentScore={score} />
            <Lives remainingLives={lives} />
            <div className="game-field">
                {invaderPositions.map((pos, index) => <Invader key={index} position={pos} />)}
                <Shooter style={shooterStyle} />
            </div>
            {gameOver && <div className="game-over">Game Over</div>}
        </div>
    );
}

export default GameBoard;
