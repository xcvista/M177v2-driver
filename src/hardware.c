
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
    GPIO_ERR = -1,
    GPIO_TIMEOUT = -2
};

enum
{
    GPIO_INPUT,
    GPIO_OUTPUT
};
#endif

static inline __attribute__((always_inline)) char *gpio_name(gpio_t pin)
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

int gpio_direction(gpio_t pin, int direction)
{
    char *name = gpio_name(pin);
    if (!name)
        return -1;
    
    if (!file_exist(name, 1))
    {
        free(name);
        return -1;
    }
    
    strcat(name, "/direction");
    switch (direction)
    {
        case GPIO_INPUT:
            file_inject(name, "in");
            break;
        case GPIO_OUTPUT:
            file_inject(name, "out");
            break;
        default:
            free(name);
            return -1;
    }
    
    free(name);
    return 0;
    
}

int gpio_read(gpio_t pin)
{
    char *name = gpio_name(pin);
    if (!name)
        return GPIO_ERR;
    
    if (!file_exist(name, 1))
    {
        free(name);
        return GPIO_ERR;
    }
    
    strcat(name, "/value");
    
    int fd = open(name, O_RDONLY);
    if (fd < 0)
    {
        free(name);
        return GPIO_ERR;
    }
    
    char status;
    read(fd, &status, 1);
    
    int rv = GPIO_ERR;
    switch (status)
    {
        case '0':
            rv = GPIO_LOW;
            break;
        case '1':
            rv = GPIO_HIGH;
            break;
        default:
            break;
    }
    
    close(fd);
    free(name);
    
    return rv;
}

int gpio_write(gpio_t pin, int value)
{
    char *name = gpio_name(pin);
    if (!name)
        return -1;
    
    if (!file_exist(name, 1))
    {
        free(name);
        return -1;
    }
    
    strcat(name, "/value");
    
    switch (value)
    {
        case GPIO_LOW:
            file_inject(name, "0");
            break;
        case GPIO_HIGH:
            file_inject(name, "1");
            break;
        default:
            free(name);
            return -1;
            break;
    }
    
    free(name);
    return 0;
}

#if XREF
typedef enum
{
    GPIO_EDGE_RISING = 1 << 0,
    GPIO_EDGE_FALLING = 1 << 1
} gpio_edge_t
#endif

int gpio_wait_irq(gpio_t pin, gpio_edge_t edge, int timeout)
{
    char *name = gpio_name(pin);
    if (!name)
        return GPIO_ERR;
    
    if (!file_exist(name, 1))
    {
        free(name);
        return GPIO_ERR;
    }
    
    char *edge_name = gpio_name(pin);
    
    strcat(name, "/value");
    strcat(edge_name, "/edge");
    
    if (!file_exist(edge_name, 0))
    {
        free(name);
        free(edge_name);
        return GPIO_ERR;
    }
    
    int fd = open(name, O_RDONLY);
    if (fd < 0)
    {
        free(name);
        free(edge_name);
        return GPIO_ERR;
    }
    
    switch (edge)
    {
        case GPIO_EDGE_RISING:
            file_inject(edge_name, "rising");
            break;
        case GPIO_EDGE_FALLING:
            file_inject(edge_name, "falling");
            break;
        case GPIO_EDGE_BOTH:
            file_inject(edge_name, "both");
            break;
        default:
            free(name);
            free(edge_name);
            return GPIO_ERR;
    }
    
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLPRI | POLLERR;
    pfd.revents = 0;
    
    int rv = poll(&pfd, 1, timeout);
    if (rv <= 0)
    {
        free(name);
        free(edge_name);
        switch (rv)
        {
            case 0:
                return GPIO_TIMEOUT;
            default:
                return GPIO_ERR;
        }
    }
    
    lseek(fd, 0, SEEK_SET);
    
    char status;
    read(fd, &status, 1);
    
    rv = GPIO_ERR;
    switch (status)
    {
        case '0':
            rv = GPIO_LOW;
            break;
        case '1':
            rv = GPIO_HIGH;
            break;
        default:
            break;
    }
    
    close(fd);
    free(name);
    free(edge_name);
    return rv;
}

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

