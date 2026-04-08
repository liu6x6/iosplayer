#include "net_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int client_connect(const char* server_ip, uint16_t port) {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        close(sock);
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        close(sock);
        return -1;
    }

    printf("Connected to server %s:%d\n", server_ip, port);
    return sock;
}

void client_send_data(int server_fd, const uint8_t* data, int size) {
    if (send(server_fd, data, size, 0) != size) {
        perror("send failed");
    }
}

int client_receive_data(int server_fd, uint8_t* buffer, int size) {
    return recv(server_fd, buffer, size, 0);
}

void client_disconnect(int server_fd) {
    close(server_fd);
}
