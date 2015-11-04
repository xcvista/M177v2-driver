#ifndef _LIBRHID_RHID_H_
#define _LIBRHID_RHID_H_

#include <librhid/hardware.h>
#include <librhid/proto.h>

__BEGIN_DECLS

int rhid_open(int i2c_addr);
void rhid_close(int fd);

int rhid_version(int fd, void *buf, size_t len);
int rhid_reset(int fd, int hard);
int rhid_get_backlight(int fd);
int rhid_set_backlight(int fd, uint8_t level);
int rhid_get_buttons(int fd);
int rhid_set_cursor(int fd, uint8_t line, uint8_t character);
int rhid_puts(int fd, const void *buf, size_t len);
int rhid_putchar(int fd, uint8_t ch);
int rhid_set_char(int fd, uint8_t ch, uint8_t bitmap[8]);

typedef enum
{
    RHID_COMMAND_CLEAR,
    RHID_COMMAND_HOME,
    RHID_COMMAND_CURSOR,
    RHID_COMMAND_NOCURSOR,
    RHID_COMMAND_BLINK,
    RHID_COMMAND_NOBLINK,
    RHID_COMMAND_DISPLAY,
    RHID_COMMAND_NODISPLAY,
    RHID_COMMAND_SCROLLLEFT,
    RHID_COMMAND_SCROLLRIGHT,
    RHID_COMMAND_TEXTLEFT,
    RHID_COMMAND_TEXTRIGHT,
    RHID_COMMAND_AUTOSCROLL,
    RHID_COMMAND_NOAUTOSCROLL
} rhid_command_t;
int rhid_command(int fd, rhid_command_t command);

__END_DECLS

#endif
