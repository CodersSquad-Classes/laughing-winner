import React from 'react';
import PropTypes from 'prop-types';
import '../Styles/Invader.css';

function Invader({ id, position, aboutToShoot }) {
    if(aboutToShoot)console.log(position)
    const invaderStyle = {
        gridColumn: position.x + 1, // CSS grid columns start at 1
        gridRow: position.y + 1, // CSS grid rows start at 1
    };

    return <div className={`invader ${aboutToShoot ? 'about-to-shoot' : ''}`} style={invaderStyle} id={`invader-${id}`}></div>;

}

Invader.propTypes = {
    id: PropTypes.number.isRequired,
    position: PropTypes.shape({
        x: PropTypes.number.isRequired,
        y: PropTypes.number.isRequired,
        id: PropTypes.number.isRequired,
    }).isRequired,
    aboutToShoot: PropTypes.bool.isRequired,
};

export default Invader;
