#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define TIMEOUT_SEC 2

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    char expr[BUFFER_SIZE];
    struct sockaddr_in servaddr;
    socklen_t len = sizeof(servaddr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("10.0.0.1"); // server IP in Mininet

    // Set receive timeout
    struct timeval tv;
    tv.tv_sec = TIMEOUT_SEC;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    while (1) {
        printf("Enter expression (e.g., 2 + 3 or sin 1.57): ");
        fgets(expr, BUFFER_SIZE, stdin);
        expr[strcspn(expr, "\n")] = 0; // remove newline

        sendto(sockfd, expr, strlen(expr), 0,
               (struct sockaddr *)&servaddr, len);

        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                         (struct sockaddr *)&servaddr, &len);
        if (n < 0) {
            printf("No response from server (possible packet loss)\n");
        } else {
            buffer[n] = '\0';
            printf("Result: %s\n", buffer);
        }
    }

    close(sockfd);
    return 0;
}

