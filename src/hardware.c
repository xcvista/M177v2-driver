
#include <librhid/hardware.h>

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

enum
{
    GPIO_INPUT,
    GPIO_OUTPUT
};
#endif

char *gpio_name(gpio_t pin)
{
    char *gpio_name = malloc(BUFSIZ);
    if (!gpio_name)
        return NULL;

    snprintf(gpio_name, BUFSIZ, "/sys/class/gpio/gpio%d", pin);
    return gpio_name;
}

int gpio_open(gpio_t pin)
{
    char *name = gpio_name(pin);
    char pinid[20];

    if (!name)
        return -1;
    
    if (file_exist(name, 1))
        return 0;

    snprintf(pinid, 20, "%d", pin);
    file_inject("/sys/class/gpio/export", pinid);

    int rv = file_exist(name, 1) ? 0 : -1;
    free(name);
    return rv;
}

int gpio_close(gpio_t pin)
{
    char *name = gpio_name(pin);
    char pinid[20];

    if (!name)
        return -1;

    if (!file_exist(name, 1))
        return 0;

    snprintf(pinid, 20, "%d", pin);
    file_inject("/sys/class/gpio/unexport", pinid);

    int rv = file_exist(name, 1) ? -1 : 0;
    free(name);
    return rv;
}

int gpio_direction(gpio_t pin, int direction);
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

int gpio_wait_irq(gpio_t pin, gpio_edge_t edge);

//------------------------------------------------------------
// I2C functions
//------------------------------------------------------------

int i2c_open(const char *devfile, uint8_t addr)
{
    int fd = open(devfile, O_RDWR);
    if (fd < 0)
        return -1;

    int rv = ioctl(fd, I2C_SLAVE, (int)addr);
    if (rv < 0)
    {
        close(fd);
        return rv;
    }

    return fd;
}

//------------------------------------------------------------
// Helper functions
//------------------------------------------------------------

int file_inject(const char *file, const char *contents)
{
    int fd = open(file, O_WRONLY | O_CREAT, 0644);
    if (fd < 0)
        return -1;

    ssize_t rv = write(fd, contents, strlen(contents));
    close(fd);
    return rv;
}

int file_exist(const char *file, int directory)
{
    struct stat st;
    if (stat(file, &st))
    {
        return 0;
    }
    if (directory && !S_ISDIR(st.st_mode))
    {
        return 0;
    }
    return 1;
}

