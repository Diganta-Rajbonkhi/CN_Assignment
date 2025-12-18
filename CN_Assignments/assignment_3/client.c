#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sockfd;
    char buffer[2048];
    struct sockaddr_in servAddr;
    socklen_t servLen = sizeof(servAddr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(9999);

    // Change to your Mininet server host IP
    inet_pton(AF_INET, "10.0.0.1", &servAddr.sin_addr);

    printf("UDP Client Ready\n");

    while (1) {
        char fruit[20], qty[10], msg[40];

        printf("\nEnter fruit name: ");
        scanf("%s", fruit);

        printf("Enter quantity: ");
        scanf("%s", qty);

        sprintf(msg, "%s,%s", fruit, qty);

        sendto(sockfd, msg, strlen(msg), 0,
               (struct sockaddr*)&servAddr, servLen);

        int n = recvfrom(sockfd, buffer, sizeof(buffer)-1, 0,
                         (struct sockaddr*)&servAddr, &servLen);

        buffer[n] = '\0';
        printf("\n--- Server Response ---\n%s\n------------------------\n", buffer);
    }

    close(sockfd);
    return 0;
}

