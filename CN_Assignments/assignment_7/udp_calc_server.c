#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

double calculate(char *expr) {
    double x, y, result;
    char op[10];

    // Check if two-number operation
    if (sscanf(expr, "%lf %s %lf", &x, op, &y) == 3) {
        if (strcmp(op, "+") == 0) return x + y;
        if (strcmp(op, "-") == 0) return x - y;
        if (strcmp(op, "*") == 0) return x * y;
        if (strcmp(op, "/") == 0) return y != 0 ? x / y : NAN;
        if (strcmp(op, "pow") == 0) return pow(x, y);
    }
    // Single-number function
    else if (sscanf(expr, "%s %lf", op, &x) == 2) {
        if (strcmp(op, "sin") == 0) return sin(x);
        if (strcmp(op, "cos") == 0) return cos(x);
        if (strcmp(op, "tan") == 0) return tan(x);
        if (strcmp(op, "sqrt") == 0) return sqrt(x);
        if (strcmp(op, "inv") == 0) return x != 0 ? 1.0 / x : NAN;
    }

    return NAN; // invalid expression
}

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
    
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);
    
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    printf("UDP Scientific Calculator Server listening on port %d...\n", PORT);
    
    while (1) {
        len = sizeof(cliaddr);
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                         (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';
        printf("Received: %s\n", buffer);
        
        double res = calculate(buffer);
        char resultStr[BUFFER_SIZE];
        if (isnan(res))
            snprintf(resultStr, BUFFER_SIZE, "Error: Invalid expression");
        else
            snprintf(resultStr, BUFFER_SIZE, "%lf", res);
        
        sendto(sockfd, resultStr, strlen(resultStr), 0,
               (struct sockaddr *)&cliaddr, len);
    }
    
    close(sockfd);
    return 0;
}

