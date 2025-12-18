#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9999);

    // Replace by server host IP in Mininet e.g., 10.0.0.1
    inet_pton(AF_INET, "10.0.0.1", &serverAddr.sin_addr);

    connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr));
    cout << "Connected to Fruit Server!\n";

    while (true) {
        string fruit, qty;
        cout << "\nEnter fruit name: ";
        cin >> fruit;

        cout << "Enter quantity: ";
        cin >> qty;

        string msg = fruit + "," + qty;
        send(sock, msg.c_str(), msg.size(), 0);

        char buffer[4096];
        int bytes = recv(sock, buffer, sizeof(buffer)-1, 0);
        if (bytes <= 0) break;

        buffer[bytes] = '\0';
        cout << "\n--- Server Response ---\n"
             << buffer << "\n------------------------\n";
    }

    close(sock);
    return 0;
}

