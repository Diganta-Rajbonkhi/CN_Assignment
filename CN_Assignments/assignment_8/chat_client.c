// chat_client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8888

int sockfd;

void *receive_messages(void *arg) {
    char buffer[1024];
    while (1) {
        int n = recv(sockfd, buffer, sizeof(buffer), 0);
        if (n > 0) {
            buffer[n] = '\0';
            printf("%s", buffer);
        }
    }
    return NULL;
}

int main() {
    struct sockaddr_in server;
    char msg[1024];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    connect(sockfd, (struct sockaddr *)&server, sizeof(server));

    printf("Connected to chat server.\n");

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_messages, NULL);

    while (1) {
        fgets(msg, sizeof(msg), stdin);
        send(sockfd, msg, strlen(msg), 0);
    }

    close(sockfd);
    return 0;
}

