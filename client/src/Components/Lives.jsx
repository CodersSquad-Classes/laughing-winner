import React from 'react';

function Lives({ remainingLives, ...props }) {
    // Render the remaining lives here.
    return (
        <div>
            Lives: {remainingLives}
        </div>
    );
}

export default Lives;
