#ifndef CLI_H
#define CLI_H

#include <getopt.h>

extern char g_out_file[256];
extern int g_device_port;

void parse_arguments(int argc, char *argv[]);

#endif // CLI_H
