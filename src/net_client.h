#ifndef NET_CLIENT_H
#define NET_CLIENT_H

#include <stdint.h>

int client_connect(const char* server_ip, uint16_t port);
void client_send_data(int server_fd, const uint8_t* data, int size);
int client_receive_data(int server_fd, uint8_t* buffer, int size);
void client_disconnect(int server_fd);

#endif // NET_CLIENT_H
