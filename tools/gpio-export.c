
#include "cli.h"

void help(void)
{
    print_version();

    eprintf("\n"
            "Usage: gpio-export <pin> ...\n"
            "Exports a GPIO pin from kernel, using sysfs interface.\n"
            "\n");
}

int main(int argc, const char *argv[])
{
    return std_gpio(argc, argv, gpio_open);
}

