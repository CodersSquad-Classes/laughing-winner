import React from 'react';
import PropTypes from 'prop-types';
import '../Styles/Invader.css'; // Import your CSS file

function Invader({ position, ...props }) {
    const invaderStyle = {
        gridColumn: position.x + 1, // CSS grid columns start at 1
        gridRow: position.y + 1, // CSS grid rows start at 1
    };

    return (
        <div className="invader" style={invaderStyle} {...props}/>
    );
}

Invader.propTypes = {
    position: PropTypes.shape({
        x: PropTypes.number.isRequired,
        y: PropTypes.number.isRequired,
    }).isRequired,
};

export default Invader;
