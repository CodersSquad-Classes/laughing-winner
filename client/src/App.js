import React, { useState } from 'react';
import GameBoard from './Components/GameBoard.jsx';
import Welcome from './Components/Welcome.jsx';
import './App.css'; // Import your CSS file

function App() {
    const [gameStarted, setGameStarted] = useState(false);
    const [numInvaders, setNumInvaders] = useState(10);

    const handlePlay = (invaders) => {
        setGameStarted(true);
        setNumInvaders(invaders);
    };

    return (
        <div className="App">
            {gameStarted ? (
                <GameBoard numInvaders={numInvaders} />
            ) : (
                <Welcome onPlay={handlePlay} />
            )}
        </div>
    );
}

export default App;
