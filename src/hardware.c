
#include "hardware.h"

#include <sys/stat.h>
#include <pthread.h>

//------------------------------------------------------------
// GPIO functions
//
// Export, Unexport pins, and basic Arduino-like interface
//------------------------------------------------------------

#if XREF
typedef uint16_t gpio_t;

enum
{
    GPIO_HIGH = 1,
    GPIO_LOW = 0,
    GPIO_ERR = -1
};
#endif

int gpio_open(gpio_t pin);
int gpio_close(gpio_t pin);
int gpio_read(gpio_t pin);
int gpio_write(gpio_t pin, int value);

#if XREF
typedef enum
{
    GPIO_EDGE_RAISING,
    GPIO_EDGE_FALLING
} gpio_edge_t
typedef void (*gpio_isr_t)(int pin, gpio_edge_t edge);
#endif

int gpio_attach_isr(gpio_t pin, gpio_edge_t edge, gpio_isr_t isr);

//------------------------------------------------------------
// I2C functions
//------------------------------------------------------------

int i2c_open(const char *devfile, uint8_t addr);

//------------------------------------------------------------
// Helper functions
//------------------------------------------------------------

int file_inject(const char *file, const char *contnets);
int file_exist(const char *file, int directory);

