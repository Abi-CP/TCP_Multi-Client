#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define MAX_BUFFER_SIZE 1024

struct ClientInfo {
    int socket;
    char username[32];
};

struct ClientInfo clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t mutex;

void printConnectedUsers() {
    printf("Connected users: ");
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket > 0) {
            printf("%s, ", clients[i].username);
        }
    }
    printf("\n");
}

void sendConnectedUsers(int client_socket) {
    char users_list[MAX_BUFFER_SIZE] = "Connected users: ";
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket > 0) {
            strcat(users_list, clients[i].username);
            strcat(users_list, ", ");
        }
    }
    strcat(users_list, "\n");
    send(client_socket, users_list, strlen(users_list), 0);
}

void broadcast(char *data) {
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket > 0) {
            send(clients[i].socket, data, strlen(data), 0);
        }
    }
    pthread_mutex_unlock(&mutex);
}

void *client_handler(void *arg) {
    int client_socket = *((int *)arg);
    char buffer[MAX_BUFFER_SIZE];
    int i, bytes_received;

    // Get the client's username
    bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received <= 0) {
        close(client_socket);
        return NULL;
    }
    buffer[bytes_received] = '\0';

    // Lock the mutex to update the client list
    pthread_mutex_lock(&mutex);
    for (i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket == 0) {
            clients[i].socket = client_socket;
            strcpy(clients[i].username, buffer);
            client_count++;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);

    printf("Client %s connected\n", buffer);
    printConnectedUsers(); // Print connected users

    char user_connected[MAX_BUFFER_SIZE] = "\n#:   ";
    strcat(user_connected, "User:\'");
    strcat(user_connected, buffer);
    strcat(user_connected, "\' JOINED!");
    strcat(user_connected, "\n");

    broadcast(user_connected);

    while (1) {
        bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            char temp[100];
            // Client has disconnected
            pthread_mutex_lock(&mutex);
            for (i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].socket == client_socket) {
                    printf("Client %s disconnected\n", clients[i].username);
                    // temp = ;
                    strcpy(temp, clients[i].username);
                    clients[i].socket = 0;
                    client_count--;
                    break;
                }

            }
            pthread_mutex_unlock(&mutex);
            close(client_socket);
            printConnectedUsers(); // Print updated connected users
            char user_disconnected[MAX_BUFFER_SIZE] = "\n#:   ";
            strcat(user_disconnected, "User:\'");
            strcat(user_disconnected, temp);
            strcat(user_disconnected, "\' has LEFT!");
            strcat(user_disconnected, "\n");

            broadcast(user_disconnected);
            return NULL;
        }
        buffer[bytes_received] = '\0';

        // Handle the "/users" command
        if (strcmp(buffer, "/users") == 0) {
            sendConnectedUsers(client_socket);
        } else {
            // Broadcast the message to all connected clients
            pthread_mutex_lock(&mutex);
            for (i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].socket > 0 && clients[i].socket != client_socket) {
                    send(clients[i].socket, buffer, bytes_received, 0);
                }
            }
            pthread_mutex_unlock(&mutex);
        }
    }

    return NULL;
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr;
    pthread_t thread_id;

    // Initialize the client list
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].socket = 0;
    }
    pthread_mutex_init(&mutex, NULL);

    // Create the server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) < 0) {
        perror("Error listening");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT);

    while (1) {
        
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);

        if (client_socket < 0) {
            perror("Error accepting connection");
            continue;
        }

        // Create a thread to handle the client
        pthread_create(&thread_id, NULL, client_handler, &client_socket);

        // Detach the thread so that it cleans up automatically
        pthread_detach(thread_id);
    }

    close(client_socket);
    close(server_socket);
    pthread_mutex_destroy(&mutex);

    return 0;
}
