const socket = new WebSocket('ws://localhost:9001');

socket.addEventListener('open', function (event) {
    socket.send('start');
});

socket.addEventListener('message', function (event) {
    console.log('Message from server: ', event.data);
    const gameState = JSON.parse(event.data);
    // TODO: Update game state in React
});

// Send game actions
function moveLeft() {
    socket.send('left');
}

function moveRight() {
    socket.send('right');
}

function shoot() {
    socket.send('shoot');
}
