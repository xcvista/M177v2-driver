
#include "cli.h"

void print_version(void)
{
    eprintf("librhid: Reduced Human Interface Device (M177v2) Driver\n"
            "Version %s Copyright (c) 2015 Max Chan\n",
            VERSION);
}


int std_gpio(int argc, const char *argv[], int (*action)(gpio_t pin))
{
    if (getuid())
    {
        eprintf("This progran have to be run by root.\n");
        exit(1);
    }

    if (argc < 2)
    {
        help();
        exit(1);
    }

    for (int idx = 1; idx < argc; idx++)
    {
        int pin = -1;
        const char *arg = argv[idx];

        if (!strcmp(arg, "-h") || !strcmp(arg, "--help"))
        {
            help();
            exit(0);
        }

        if (!sscanf(arg, "%i", &pin) || pin < 0)
        {
            eprintf("Cannot parse argument, skipping: %s\n", arg);
            continue;
        }

        if (action(pin))
        {
            perror(NULL);
            exit(1);
        }
    }
    
    return 0;
}

