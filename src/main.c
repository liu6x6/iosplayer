#include "cli.h"
#include "mux.h"
#include "player.h"
#include "http_client.h"

#include <libavformat/avformat.h>

int main(int argc, char *argv[]) {
    parse_arguments(argc, argv);

    avformat_network_init();
    http_client_init();

    int mux_fd = -1;
    if (connect_usbmuxd(g_device_port, &mux_fd) < 0) {
        http_client_cleanup();
        return -1;
    }

    run_player(mux_fd);

    http_client_cleanup();
    return 0;
}
