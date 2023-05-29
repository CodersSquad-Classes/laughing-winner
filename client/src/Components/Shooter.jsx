import React from 'react';

function Shooter({ position, ...props }) {
    // Render your shooter here, based on the current state.
    return (
        <div style={props.style}>
            Shooter
        </div>
    );
}

export default Shooter;
