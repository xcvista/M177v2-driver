#ifndef CLI_H
#define CLI_H

#include <librhid/hardware.h>
#include <getopt.h>
#include <stdio.h>

__BEGIN_DECLS

#define eprintf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)

void print_version(void);

void help(void);
int std_gpio(int argc, const char *argv[], int (*action)(gpio_t gpio));

__END_DECLS

#endif
