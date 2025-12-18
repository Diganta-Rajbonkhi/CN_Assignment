# TCP Client–Server Communication in C

## 📌 Overview

This project demonstrates a simple **TCP client–server communication** using **C socket programming**. The server waits for a client connection on a specific port. Once connected, the client sends a message to the server, and the server replies back.

This program is useful for understanding the basics of:

* TCP socket creation
* Client–server architecture
* Data transmission using sockets

---

## 📁 Project Files

| File Name      | Description            |
| -------------- | ---------------------- |
| `tcp_server.c` | TCP server source code |
| `tcp_client.c` | TCP client source code |
| `README.md`    | Project documentation  |

---

## ⚙️ Requirements

* Linux / Ubuntu (or Mininet environment)
* GCC compiler
* Basic knowledge of C programming and networking

---

## 🔧 Compilation

Compile both programs using GCC:

```bash
gcc tcp_server.c -o server
gcc tcp_client.c -o client
```

---

## ▶️ Execution Steps

### Step 1: Run the Server

Open a terminal and execute:

```bash
./server
```

Expected output:

```
Server is waiting for connection...
```

---

### Step 2: Run the Client

Open another terminal (or another Mininet host) and run:

```bash
./client
```

---

## 🔄 Communication Flow

1. The server creates a socket, binds it to port **8080**, and listens for connections.
2. The client creates a socket and connects to the server using IP address `10.0.0.1`.
3. The client sends the message **"Hi"** to the server.
4. The server receives the message and displays it.
5. The server sends back **"Hello"** to the client.
6. The client receives and prints the server response.

---
## 🖼️ Screenshot

![TCP Client Server Output](assignment1.png)


## 📤 Sample Output

### Server Side

```
Server is waiting for connection...
Client says: Hi
```

### Client Side

```
Server says: Hello
```

---

## 🧠 Concepts Used

* TCP (Transmission Control Protocol)
* Socket programming in C
* Client–Server model
* IPv4 networking

---

## 📌 Notes

* Start the server before running the client.
* Ensure port **8080** is not in use.
* IP address `10.0.0.1` is typically used in **Mininet** environments.

---

## ✅ Conclusion

This program successfully demonstrates basic TCP communication between a client and a server using C. It forms a strong foundation for understanding network programming concepts.
