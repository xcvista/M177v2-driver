
#include <librhid/rhid.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
    if(getuid())
        return 1;

    int fd = rhid_open(0x46);
    if (fd < 0)
        return 2;

    int button = -1;
    rhid_event_t events = rhid_event_begin(fd);
    if (!events)
    {
        rhid_close(fd);
        return -3;
    }

    char buffer[16] = {0};
    while ((button = rhid_event(events)))
    {
        if (button < 0)
            continue;
        
        snprintf(buffer, 16, "0x%02x", button);
        puts(buffer);
        rhid_clear(fd);
        rhid_puts(fd, buffer, strlen(buffer));
    }

    rhid_event_end(events);
    rhid_close(fd);
}
