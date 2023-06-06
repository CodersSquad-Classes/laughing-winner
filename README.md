# Game Project

This project is a game implemented in C, using WebSocket communication for real-time interaction. It features invaders that the player can shoot down with a shooter controlled through WebSocket commands.

## Dependencies

Before running this project, make sure you have the following dependencies installed:

1. libwebsockets: This is a C library that provides support for WebSocket communication. Make sure you have libwebsockets installed on your system. You can find more information about libwebsockets, including installation instructions, on the official website: [https://libwebsockets.org/](https://libwebsockets.org/)

2. OpenSSL: This project uses OpenSSL for cryptographic operations. Make sure you have OpenSSL installed on your system. You can find more information about OpenSSL, including installation instructions, on the official website: [https://www.openssl.org/](https://www.openssl.org/)

3. Node.js and npm: This project includes a frontend component built with React. Make sure you have Node.js and npm (Node Package Manager) installed on your system. You can download Node.js from the official website: [https://nodejs.org/](https://nodejs.org/)

## Compilation

To compile the project, use the following command:

gcc -o game queueStruct.c game.c websocket_server.c -I <path_to_libwebsockets_include> -I <path_to_openssl_include> -L <path_to_libwebsockets_lib> -L <path_to_openssl_lib> -lpthread -lwebsockets


Replace `<path_to_libwebsockets_include>`, `<path_to_openssl_include>`, `<path_to_libwebsockets_lib>`, and `<path_to_openssl_lib>` with the actual paths where the header files and libraries are located on your system.

## Frontend Setup

The frontend component of this project is built with React. To set up the frontend, navigate to the `frontend` directory and run the following command:

npm install


This will install all the necessary dependencies for the React app.

## Running the Game

After compiling the project and setting up the frontend, you can run the game by executing the generated `game` executable. Make sure to start the WebSocket server first before launching the frontend.

To start the WebSocket server, run:

./game


Next, navigate to the `frontend` directory and run:

npm start


This will start the development server for the React app. You can access the game in a web browser by visiting [http://localhost:3000/](http://localhost:3000/).

## Game Instructions

- Use the left and right arrow keys to move the shooter.
- Press the spacebar to shoot at the invaders.
- The goal is to shoot down all the invaders before they reach the bottom of the screen.
