#include "net_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

int server_start(uint16_t port) {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        return -1;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(server_fd);
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, 1) < 0) { // Listen for only one client
        perror("listen");
        close(server_fd);
        return -1;
    }

    printf("Server listening on port %d\n", port);
    return server_fd;
}

int server_accept_client(int server_fd) {
    int client_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        return -1;
    }

    printf("Client connected\n");
    return client_fd;
}

void server_send_data(int client_fd, const uint8_t* data, int size) {
    if (send(client_fd, data, size, 0) != size) {
        perror("send failed");
    }
}

int server_receive_data(int client_fd, uint8_t* buffer, int size) {
    return recv(client_fd, buffer, size, 0);
}

void server_stop(int server_fd) {
    close(server_fd);
}
