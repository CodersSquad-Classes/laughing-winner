import React from 'react';
import PropTypes from 'prop-types';
import '../Styles/Shooter.css'; // Import your CSS file

function Shooter({ style, shooting }) {
    return (
        <div className={`shooter ${shooting ? 'shooting' : ''}`} style={style}>
            <div className="triangle"></div>
        </div>
    );
}

Shooter.propTypes = {
    style: PropTypes.object.isRequired,
    shooting: PropTypes.bool.isRequired,
};

export default Shooter;
