#include "cli.h"
#include "net_client.h"
#include "player.h"

#include <libavformat/avformat.h>

#define DEFAULT_SERVER_IP "127.0.0.1"
#define DEFAULT_SERVER_PORT 12345

int main(int argc, char *argv[]) {
    // We can extend cli.c to parse a server IP, but for now, we'll use a default.
    parse_arguments(argc, argv);

    avformat_network_init();

    int server_fd = client_connect(DEFAULT_SERVER_IP, DEFAULT_SERVER_PORT);
    if (server_fd < 0) {
        return -1;
    }

    // The player will now run with the server's file descriptor
    run_player(server_fd);

    client_disconnect(server_fd);

    return 0;
}
