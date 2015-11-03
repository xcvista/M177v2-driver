
#include "cli.h"

void help(void)
{
    print_version();

    eprintf("\n"
            "Usage: gpio-read <pin> ...\n"
            "Reads the value of a GPIO pin, using sysfs interface.\n"
            "\n");
}

int gpioread(gpio_t);

int main(int argc, const char *argv[])
{
    return std_gpio(argc, argv, gpioread);
}

int gpioread(gpio_t pin)
{
    int value = gpio_read(pin);

    if (value < 0)
        return value;

    printf("%d: %d\n", pin, value);
    return 0;
}

