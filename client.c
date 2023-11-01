#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ncurses.h>

#define PORT 8080
#define MAX_BUFFER_SIZE 1024

char SERVER_IP[15];


int client_socket;
char username[32];

void *receive_messages(void *arg) {
    int bytes_received;
    char buffer[MAX_BUFFER_SIZE];

    while (1) {
        bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            perror("Server disconnected");
            close(client_socket);
            exit(EXIT_FAILURE);
        }

        buffer[bytes_received] = '\0';
        printf("%s\n", buffer);
    }
}


int main() {
    initscr(); // Initialize ncurses

    printw("Do you want to use a specified IP? (Y/N):\n");
    refresh();

    char choice;
    scanw(" %c", &choice);
    char confirmChoice = 'N';

    switch (choice) {
        case 'y':
        case 'Y':
            while (confirmChoice != 'y' && confirmChoice != 'Y') {
                printw("Enter the server IP: ");
                refresh();
                getstr(SERVER_IP);
                printw("IP: %s\nConfirm? (Y/N): ", SERVER_IP);
                refresh();
                scanw(" %c", &confirmChoice);
            }
            break;

        default:
            printw("\nSwitched to local host: IP 127.0.0.1\n");
            refresh();
            strcpy(SERVER_IP, "127.0.0.1");
            break;
    }

    clear(); // Clear the screen
    refresh();
    // getch(); // Wait for a key press

    endwin(); // End ncurses
    struct sockaddr_in server_addr;
    pthread_t receive_thread;
    char message[MAX_BUFFER_SIZE];

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Enter your username: ");
    fgets(username, sizeof(username), stdin);

    size_t len = strlen(username);
    if (len > 0 && username[len - 1] == '\n') {
        username[len - 1] = '\0';
    }

    send(client_socket, username, strlen(username), 0);

    pthread_create(&receive_thread, NULL, receive_messages, NULL);

    while (1) {
        fgets(message, sizeof(message), stdin);

        len = strlen(message);
        if (len > 0 && message[len - 1] == '\n') {
            message[len - 1] = '\0';
        }

        if (strncmp(message, "/", 1) == 0) {
            if (strcmp(message, "/code") == 0) {
                printf("\nYou're in Code mode, your messages will be sent without a username.\nUse \'/q' to quit code mode.\n");
                printf("... You (Started)...\n\n");
                snprintf(message, sizeof(message), "\n... #%s (Started Code Mode) ...\n\n", username);
                send(client_socket, message, strlen(message), 0);
                while (1) {
                    fgets(message, sizeof(message), stdin);
                    size_t cLen = strlen(message);
                    if (cLen > 0) {
                        message[cLen - 1] = '\0';
                    }
                    if (strcmp(message, "/q") == 0) {
                        printf("\n... You (Ended) ...\n\n");
                        break;
                    }
                    send(client_socket, "\n", strlen("\n"), 0);
                    send(client_socket, message, strlen(message), 0);
                }

                snprintf(message, sizeof(message), "\n... #%s (Ended Code Mode) ...\n", username);
                send(client_socket, message, strlen(message), 0);
                continue;
            }

            if (strcmp(message, "/users") == 0) {
                send(client_socket, "/users", strlen("/users"), 0);
            } else if (strcmp(message, "/help") == 0) {
                printf("\n");
                printf("#:   /code  - Send only a message.\n");
                printf("#:   /exit  - Leave the chat.\n");
                printf("#:   /users - Display connected users.\n");
            } else if (strcmp(message, "/exit") == 0) {
                exit(0);
            } else {
                printf("\n#:   Invalid Command... Use \'/help' for help.\n");
            }
        } else {
            char full_message[MAX_BUFFER_SIZE + 64];
            snprintf(full_message, sizeof(full_message), "%s: %s", username, message);
            send(client_socket, full_message, strlen(full_message), 0);
        }
    }

    close(client_socket);
    // refresh();
    // printf("Press any key...");
    // getch(); // Wait for a key press

    // endwin(); // End ncurses

    return 0;
}
