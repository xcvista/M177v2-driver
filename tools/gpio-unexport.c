
#include "cli.h"

void help(void)
{
    print_version();

    eprintf("\n"
            "Usage: gpio-unexport <pin> ...\n"
            "Returns a GPIO pin to kernel, using sysfs interface.\n"
            "\n");
}

int main(int argc, const char *argv[])
{
    return std_gpio(argc, argv, gpio_close);
}

