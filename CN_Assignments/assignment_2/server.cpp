#include <iostream>
#include <string>
#include <map>
#include <set>
#include <ctime>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <mutex>

using namespace std;

struct FruitRecord {
    int quantity;
    int last_sold;
    string timestamp;
};

map<string, FruitRecord> fruits = {
    {"apple",  {50, 0, "None"}},
    {"banana", {30, 0, "None"}},
    {"orange", {40, 0, "None"}}
};

set<string> customers;   // Store unique <IP:port>
mutex mtx;

string currentTime() {
    time_t now = time(0);
    return string(ctime(&now));
}

void handleClient(int clientSocket, string clientID) {
    char buffer[1024];

    {
        lock_guard<mutex> lock(mtx);
        customers.insert(clientID);
    }

    while (true) {
        int bytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) break;

        buffer[bytes] = '\0';
        string data(buffer);

        // Expect format: fruit,qty
        size_t commaPos = data.find(',');
        if (commaPos == string::npos) continue;

        string fruit = data.substr(0, commaPos);
        int qty = stoi(data.substr(commaPos + 1));

        string response;

        {
            lock_guard<mutex> lock(mtx);

            if (fruits.find(fruit) != fruits.end()) {
                FruitRecord &rec = fruits[fruit];

                if (rec.quantity >= qty) {
                    rec.quantity -= qty;
                    rec.last_sold = qty;
                    rec.timestamp = currentTime();

                    response = "Purchase successful!\nRemaining " + fruit +
                               ": " + to_string(rec.quantity) + "\n";
                } else {
                    response = "Regret: Only " + 
                               to_string(rec.quantity) + " left.\n";
                }
            } else {
                response = "Fruit not found!\n";
            }

            // Add customer details
            response += "\nUnique customers so far: " + 
                        to_string(customers.size()) + "\nCustomer List:\n";

            for (auto &c : customers)
                response += c + "\n";
        }

        send(clientSocket, response.c_str(), response.size(), 0);
    }

    close(clientSocket);
    cout << "[DISCONNECTED] " << clientID << endl;
}

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9999);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 5);

    cout << "[SERVER RUNNING] Listening on port 9999...\n";

    while (true) {
        sockaddr_in clientAddr{};
        socklen_t clientSize = sizeof(clientAddr);

        int clientSocket = accept(serverSocket, 
                                  (sockaddr*)&clientAddr, &clientSize);

        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, ip, sizeof(ip));
        int port = ntohs(clientAddr.sin_port);

        string clientID = string(ip) + ":" + to_string(port);

        cout << "[CONNECTED] " << clientID << endl;

        thread(handleClient, clientSocket, clientID).detach();
    }

    close(serverSocket);
    return 0;
}

