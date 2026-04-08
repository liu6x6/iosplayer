#include "mux.h"
#include <stdio.h>
#include <usbmuxd.h>
#include <sys/socket.h>
#include <libavformat/avformat.h> // For AVERROR_EOF

int connect_usbmuxd(uint16_t port, int* device_fd) {
    usbmuxd_device_info_t *dev_list = NULL;
    int count = usbmuxd_get_device_list(&dev_list);

    if (count < 0) {
        fprintf(stderr, "Failed to get device list (is usbmuxd running?)\n");
        return -1;
    }

    if (count == 0) {
        fprintf(stderr, "No iOS devices connected\n");
        usbmuxd_device_list_free(&dev_list);
        return -1;
    }

    printf("Found %d device(s)\n", count);
    // Use the first device
    usbmuxd_device_info_t *dev = &dev_list[0];
    printf("Connecting to device with UDID: %s, port %d\n", dev->udid, port);

    *device_fd = usbmuxd_connect(dev->handle, port);
    if (*device_fd < 0) {
        fprintf(stderr, "Failed to connect to device port %d\n", port);
        usbmuxd_device_list_free(&dev_list);
        return -1;
    }

    printf("Successfully connected to device port %d, got fd %d\n", port, *device_fd);
    usbmuxd_device_list_free(&dev_list);
    return 0; // success
}

int read_packet_usbmuxd(void *opaque, uint8_t *buf, int buf_size) {
    int fd = (intptr_t)opaque;
    int ret = recv(fd, buf, buf_size, 0);
    if (ret < 0) {
        // When the connection is closed by the peer, recv can return -1
        // and errno will be set. In many cases, we can just treat it as EOF.
        return AVERROR_EOF;
    }
    if (ret == 0) {
        // EOF
        return AVERROR_EOF;
    }
    return ret;
}
