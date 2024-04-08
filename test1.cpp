#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <vector>
#include <algorithm>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket, max_sd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    std::vector<int> clients(MAX_CLIENTS, 0);

    // Create a socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the specified port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    // Accept incoming connections and manage them asynchronously
    while (true) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(server_fd, &read_fds);
        max_sd = server_fd;

        for (auto const &client : clients) {
            if (client > 0) {
                FD_SET(client, &read_fds);
                max_sd = std::max(max_sd, client);
            }
        }

        int activity = select(max_sd + 1, &read_fds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR)) {
            perror("Select error");
        }

        if (FD_ISSET(server_fd, &read_fds)) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                perror("Accept failed");
                exit(EXIT_FAILURE);
            }

            std::cout << "New connection, socket fd is " << new_socket << ", IP is: " << inet_ntoa(address.sin_addr) << ", port is: " << ntohs(address.sin_port) << std::endl;

            // Add new socket to array of clients
            for (size_t i = 0; i < clients.size(); ++i) {
                if (clients[i] == 0) {
                    clients[i] = new_socket;
                    break;
                }
            }
        }

        for (auto &client : clients) {
            if (FD_ISSET(client, &read_fds)) {
                int valread = read(client, buffer, BUFFER_SIZE);
                if (valread == 0) {
                    // Client disconnected
                    std::cout << "Host disconnected, socket fd is " << client << std::endl;
                    close(client);
                    client = 0;
                } else {
                    // Process request and send response
                    std::cout << "Received request from client " << client << ": " << buffer << std::endl;
                    // Implement your request processing logic here
                    send(client, "Response from server", strlen("Response from server"), 0);
                }
            }
        }
    }

    return 0;
}