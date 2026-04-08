#include "cli.h"
#include "mux.h"
#include "net_server.h"
#include "http_client.h"

#include <libavformat/avformat.h>
#include <unistd.h> // for close()

#define DEFAULT_SERVER_PORT 12345

// A simple protocol for control commands
// We'll expand this later
void handle_control_commands(int client_fd) {
    char buffer[1024];
    int n = server_receive_data(client_fd, (uint8_t*)buffer, sizeof(buffer) - 1);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Received command: %s", buffer);

        double x, y, fromX, fromY, toX, toY, duration;
        if (sscanf(buffer, "TAP %lf %lf", &x, &y) == 2) {
            send_tap_request(x, y);
        } else if (sscanf(buffer, "SWIPE %lf %lf %lf %lf %lf", &fromX, &fromY, &toX, &toY, &duration) == 5) {
            send_swipe_request(fromX, fromY, toX, toY, duration);
        } else if (strncmp(buffer, "KEYS ", 5) == 0) {
            send_keys_request(buffer + 5);
        }
    }
}

int main(int argc, char *argv[]) {
    parse_arguments(argc, argv);

    avformat_network_init();
    http_client_init();

    int mux_fd = -1;
    if (connect_usbmuxd(g_device_port, &mux_fd) < 0) {
        http_client_cleanup();
        return -1;
    }

    int server_fd = server_start(DEFAULT_SERVER_PORT);
    if (server_fd < 0) {
        close(mux_fd);
        http_client_cleanup();
        return -1;
    }

    while (1) {
        int client_fd = server_accept_client(server_fd);
        if (client_fd < 0) {
            continue; // Wait for another client
        }

        // For simplicity, this server handles one client at a time.
        // We can use threads or select() to handle multiple clients.
        
        // This is a simplified loop. We should use select() or poll() 
        // to handle mux_fd and client_fd simultaneously.
        while(1) {
            // Forward video data
            uint8_t video_buffer[4096];
            int n = read(mux_fd, video_buffer, sizeof(video_buffer));
            if (n > 0) {
                server_send_data(client_fd, video_buffer, n);
            } else {
                printf("Disconnected from device.");
                break;
            }

            // Check for control commands (in a non-blocking way)
            // For a real-world app, we'd use select() or poll() here.
            handle_control_commands(client_fd);
        }

        close(client_fd);
        printf("Client disconnected. Waiting for new connection...\n");
    }

    server_stop(server_fd);
    close(mux_fd);
    http_client_cleanup();

    return 0;
}
