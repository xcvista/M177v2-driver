
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
    
    if (!sscanf(val, "%i", &pv) || pv < 0 || pv > 1)
    {
        if (!strcmp(val, "input"))
        {
            pv = GPIO_INPUT;
        }
        else if (!strcmp(val, "output"))
        {
            pv = GPIO_OUTPUT;
        }
        else
        {
            eprintf("Cannot parse argument, skipping: %s\n", val);
            exit(1);
        }
    }
    
    if (gpio_direction(pin, pv))
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
            "Usage: gpio-mode <pin> <value>\n"
            "Writes the value of a GPIO pin, using sysfs interface.\n"
            "\n"
            "Value is one of \"input\" and \"output\"\n"
            "\n");
}

