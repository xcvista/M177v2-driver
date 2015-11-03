
#include "cli.h"

int main(int argc, const char *argv[])
{
    if (getuid())
    {
        eprintf("This progran have to be run by root.\n");
        exit(1);
    }
    
    if (argc < 3)
    {
        help();
        exit(1);
    }
    
    int pin = -1;
    int pv = -1;
    const char *arg = argv[1];
    const char *val = argv[2];
    
    if (!strcmp(arg, "-h") || !strcmp(arg, "--help") ||
        !strcmp(val, "-h") || !strcmp(val, "--help"))
    {
        help();
        exit(0);
    }
    
    if (!sscanf(arg, "%i", &pin) || pin < 0)
    {
        eprintf("Cannot parse argument, skipping: %s\n", arg);
        exit(1);
    }
    
    if (!sscanf(val, "%i", &pv) || pv < 0 || pv > 2)
    {
        if (!strcmp(val, "rising"))
        {
            pv = GPIO_EDGE_RISING;
        }
        else if (!strcmp(val, "falling"))
        {
            pv = GPIO_EDGE_FALLING;
        }
        else if (!strcmp(val, "both"))
        {
            pv = GPIO_EDGE_BOTH;
        }
        else
        {
            eprintf("Cannot parse argument, skipping: %s\n", val);
            exit(1);
        }
    }
    
    int current_pin = gpio_read(pin);
    
    if ((current_pin == 0 && pv == GPIO_EDGE_FALLING) ||
        (current_pin == 1 && pv == GPIO_EDGE_RISING))
    {
        exit(0);
    }
    
    if (gpio_wait_irq(pin, pv, -1))
    {
        perror(NULL);
        exit(1);
    }
    
    return 0;
}

void help(void)
{
    print_version();
    
    eprintf("\n"
            "Usage: gpio-wait <pin> <edge>\n"
            "Wait for the GPIO pin interface to interrupt, using sysfs interface.\n"
            "\n"
            "Edge is one of \"rising\", \"falling\" and \"both\"\n"
            "\n");
}

