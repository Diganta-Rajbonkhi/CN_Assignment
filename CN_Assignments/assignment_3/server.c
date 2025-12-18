#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h> 

struct Fruit {
    int quantity;
    int last_sold;
    char timestamp[50];
};

struct Fruit fruits[3] = {
    {50, 0, "None"},   // apple
    {30, 0, "None"},   // banana
    {40, 0, "None"}    // orange
};

char customers[100][50];   // store up to 100 customers
int customerCount = 0;

int isNewCustomer(char *clientID) {
    for (int i = 0; i < customerCount; i++) {
        if (strcmp(customers[i], clientID) == 0)
            return 0;  // already exists
    }
    return 1;  // new customer
}

char* timeNow() {
    time_t t;
    time(&t);
    return ctime(&t);
}

int main() {
    int sockfd;
    char buffer[1024];
    struct sockaddr_in servAddr, cliAddr;
    socklen_t cliLen = sizeof(cliAddr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(9999);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr));

    printf("[UDP SERVER RUNNING] Port 9999\n");

    while (1) {
        int n = recvfrom(sockfd, buffer, sizeof(buffer)-1, 0,
                         (struct sockaddr*)&cliAddr, &cliLen);
        buffer[n] = '\0';

        char clientID[50];
        sprintf(clientID, "%s:%d", inet_ntoa(cliAddr.sin_addr), ntohs(cliAddr.sin_port));

        if (isNewCustomer(clientID)) {
            strcpy(customers[customerCount], clientID);
            customerCount++;
        }

        char fruit[20];
        int qty;

        sscanf(buffer, "%[^,],%d", fruit, &qty);

        int index = -1;
        if (strcmp(fruit, "apple") == 0) index = 0;
        else if (strcmp(fruit, "banana") == 0) index = 1;
        else if (strcmp(fruit, "orange") == 0) index = 2;

        char response[2048];
        strcpy(response, "");

        if (index != -1) {
            if (fruits[index].quantity >= qty) {
                fruits[index].quantity -= qty;
                fruits[index].last_sold = qty;
                strcpy(fruits[index].timestamp, timeNow());

                sprintf(response,
                        "Purchase successful!\nRemaining %s: %d\n",
                        fruit, fruits[index].quantity);
            } else {
                sprintf(response,
                        "Regret: Only %d %s left.\n",
                        fruits[index].quantity, fruit);
            }
        } else {
            sprintf(response, "Fruit not found!\n");
        }

        // Append customer details
        sprintf(response + strlen(response),
                "\nUnique customers: %d\nCustomer List:\n",
                customerCount);

        for (int i = 0; i < customerCount; i++) {
            sprintf(response + strlen(response), "%s\n", customers[i]);
        }

        sendto(sockfd, response, strlen(response), 0,
               (struct sockaddr*)&cliAddr, cliLen);
    }

    close(sockfd);
    return 0;
}

