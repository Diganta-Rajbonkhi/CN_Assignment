#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <receiver_ip> <port>\n", argv[0]);
        return -1;
    }

    int sockfd;
    struct sockaddr_in server_addr;
    char message[1024] = "Hello from C UDP packet generator!";

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    // Send 100 packets with 50ms delay
    for (int i = 0; i < 100; i++) {
        sendto(sockfd, message, strlen(message), 0,
               (struct sockaddr *)&server_addr, sizeof(server_addr));
        printf("Packet %d sent\n", i + 1);
        usleep(50000); // 50 ms
    }

    close(sockfd);
    return 0;
}

