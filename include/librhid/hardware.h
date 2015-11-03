
#ifndef _RHID_HARDWARE_H_
#define _RHID_HARDWARE_H_

#include <linux/i2c-dev.h>
#include <sys/cdefs.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <inttypes.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <poll.h>

__BEGIN_DECLS

//------------------------------------------------------------
// GPIO functions
//
// Export, Unexport pins, and basic Arduino-like interface
//------------------------------------------------------------

typedef uint16_t gpio_t;

enum
{
    GPIO_HIGH = 1,
    GPIO_LOW = 0,
    GPIO_ERR = -1,
    GPIO_TIMEOUT = -2
};

enum
{
    GPIO_INPUT,
    GPIO_OUTPUT
};

int gpio_open(gpio_t pin);
int gpio_close(gpio_t pin);
int gpio_direction(gpio_t pin, int direction);
int gpio_read(gpio_t pin);
int gpio_write(gpio_t pin, int value);

typedef enum
{
    GPIO_EDGE_RISING = 1 << 0,
    GPIO_EDGE_FALLING = 1 << 1,
    GPIO_EDGE_BOTH = GPIO_EDGE_RISING | GPIO_EDGE_FALLING
} gpio_edge_t;

int gpio_wait_irq(gpio_t pin, gpio_edge_t edge, int timeout);

//------------------------------------------------------------
// I2C functions
//------------------------------------------------------------

int i2c_open(const char *devfile, uint8_t addr);

//------------------------------------------------------------
// Helper functions
//------------------------------------------------------------

int file_inject(const char *file, const char *contnets);
int file_exist(const char *file, int directory);

__END_DECLS

#endif
