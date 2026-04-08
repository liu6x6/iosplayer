#ifndef NET_SERVER_H
#define NET_SERVER_H

#include <stdint.h>

int server_start(uint16_t port);
int server_accept_client(int server_fd);
void server_send_data(int client_fd, const uint8_t* data, int size);
int server_receive_data(int client_fd, uint8_t* buffer, int size);
void server_stop(int server_fd);

#endif // NET_SERVER_H
