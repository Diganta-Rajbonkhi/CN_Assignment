// chat_server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define PORT 8888
#define MAX_CLIENTS 100

int client_sockets[MAX_CLIENTS];
pthread_mutex_t lock;

void timestamp(char *buffer) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    sprintf(buffer, "[%02d:%02d:%02d] ", t->tm_hour, t->tm_min, t->tm_sec);
}

void broadcast_message(char *msg, int current_client) {
    pthread_mutex_lock(&lock);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] != 0 && client_sockets[i] != current_client) {
            send(client_sockets[i], msg, strlen(msg), 0);
        }
    }

    pthread_mutex_unlock(&lock);
}

void save_log(char *msg) {
    FILE *log = fopen("log.txt", "a");
    if (!log) return;
    fprintf(log, "%s", msg);
    fclose(log);
}

void *client_handler(void *socket_desc) {
    int sock = *(int *)socket_desc;
    char msg[1024];

    while (1) {
        memset(msg, 0, sizeof(msg));

        int read_size = recv(sock, msg, sizeof(msg), 0);
        if (read_size <= 0) {
            printf("Client disconnected\n");
            close(sock);

            // Remove from client list
            pthread_mutex_lock(&lock);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == sock)
                    client_sockets[i] = 0;
            }
            pthread_mutex_unlock(&lock);

            pthread_exit(NULL);
        }

        // Add timestamp
        char formatted_msg[1200];
        char timebuf[32];
        timestamp(timebuf);

        sprintf(formatted_msg, "%sClient[%d]: %s", timebuf, sock, msg);

        // Save to log
        save_log(formatted_msg);

        // Broadcast to everyone
        broadcast_message(formatted_msg, sock);

        printf("%s", formatted_msg);
    }

    return NULL;
}

int main() {
    int server_fd, new_socket, c;
    struct sockaddr_in server, client;
    pthread_t thread_id;

    pthread_mutex_init(&lock, NULL);

    // Prepare socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&server, sizeof(server));
    listen(server_fd, 5);

    printf("Chat server started on port %d...\n", PORT);

    c = sizeof(struct sockaddr_in);

    while ((new_socket = accept(server_fd, (struct sockaddr *)&client, (socklen_t *)&c))) {
        printf("Client connected. Socket ID = %d\n", new_socket);

        // Add client to list
        pthread_mutex_lock(&lock);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] == 0) {
                client_sockets[i] = new_socket;
                break;
            }
        }
        pthread_mutex_unlock(&lock);

        // Create thread
        if (pthread_create(&thread_id, NULL, client_handler, &new_socket) < 0) {
            perror("Could not create thread");
            return 1;
        }
    }

    pthread_mutex_destroy(&lock);
    return 0;
}

