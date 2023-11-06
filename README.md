# Simple Multi-Client Chat Application using TCP in C

This is a simple chat application consisting of a client and a server written in C. The server listens for incoming client connections and allows users to chat with each other. Users can join the chat, send messages, list connected users, and exit gracefully.

## Table of Contents

- [Client](#client)
- [Server](#server)
- [Usage](#usage)
- [Contributing](#contributing)

## Client

The client program is responsible for connecting to the chat server and sending and receiving messages. It provides the following features:

- Ability to specify the server IP or connect to the localhost.
- User-defined username for identification in the chat.
- Sending and receiving chat messages with other users.
- Special commands such as "/code" for code mode and "/users" to list connected users.
- Graceful exit with the "/exit" command.

To use the client, you can compile the code and run the executable. Follow the prompts to specify the server IP (or use the default localhost) and set your username. You can then start sending and receiving messages in the chat.

## Server

The server program is responsible for accepting client connections, managing the list of connected users, and broadcasting messages to all connected clients. It provides the following features:

- Accepting multiple client connections and handling them in separate threads.
- Keeping track of connected users and their usernames.
- Broadcasting chat messages to all connected clients.
- Handling special commands like "/users" to list connected users.

To use the server, you can compile the code and run the executable. It will start listening on a specified port (default is 8080) for incoming client connections. Once connected, clients can send and receive messages in the chat.

## Usage

1. Compile the client and server programs using a C compiler such as `gcc`. For example:
   
   ```bash
   gcc -o client client.c -lpthread -lncurses
   gcc -o server server.c -lpthread
   ```

2. Run the server program.

   ```bash
   ./server [port]
   ```

3. (For Multi-Device Chatting) Note the IP address of your device (server) in network settings. (Skip this step for running server and client in same device)

4. Connect all your Client devices in same network.

5. Run the client program.

   ```bash
   ./client
   ```

6. Follow the prompts to specify the server IP (or use the default localhost by entering 'N' in the promt for Specified IP) and set your username.
   
7. Start sending and receiving messages in the chat. You can use special commands like "/code" to enter code mode, "/users" to list connected users, and "/exit" to leave the chat.

## Contributing

If you want to contribute to this project, feel free to fork the repository and make improvements. You can open issues for bug reports or feature requests and create pull requests to submit your changes. Your contributions are highly appreciated!

## License

This project is licensed under the [MIT License](LICENSE). Feel free to use and modify the code for your own purposes.
