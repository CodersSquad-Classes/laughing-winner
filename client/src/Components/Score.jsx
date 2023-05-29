import React from 'react';

function Score({ currentScore, ...props }) {
    // Render the current score here.
    return (
        <div>
            Score: {currentScore}
        </div>
    );
}

export default Score;
