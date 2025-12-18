#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return -1;
    }

    int sockfd;
    struct sockaddr_in addr, client_addr;
    char buffer[1024];
    socklen_t addr_len = sizeof(client_addr);

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Bind socket
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[1]));
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    printf("Receiver started on port %s…\n", argv[1]);

    while (1) {
        int n = recvfrom(sockfd, buffer, sizeof(buffer)-1, 0,
                         (struct sockaddr *)&client_addr, &addr_len);
        if (n > 0) {
            buffer[n] = '\0';
            printf("Received: %s\n", buffer);
        }
    }

    close(sockfd);
    return 0;
}

