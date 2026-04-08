#ifndef MUX_H
#define MUX_H

#include <stdint.h>

int connect_usbmuxd(uint16_t port, int* device_fd);
int read_packet_usbmuxd(void *opaque, uint8_t *buf, int buf_size);

#endif // MUX_H
