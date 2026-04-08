#include "cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char g_out_file[256] = "";
int g_device_port = 10001; // Default device port

void parse_arguments(int argc, char *argv[]) {
    int opt;
    struct option long_options[] = {
        {"out",  required_argument, 0, 'o'},
        {"port", required_argument, 0, 'p'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "o:p:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'o':
                strncpy(g_out_file, optarg, sizeof(g_out_file) - 1);
                g_out_file[sizeof(g_out_file) - 1] = '\0';
                break;
            case 'p':
                g_device_port = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-o output_file] [-p device_port]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
}
