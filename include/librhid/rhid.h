#ifndef _LIBRHID_RHID_H_
#define _LIBRHID_RHID_H_

#include <sys/cdefs.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdint.h>
#include <sys/types.h>

__BEGIN_DECLS

/// @name RHID tools

int rhid_open(int i2c_addr);
void rhid_close(int fd);

/// @name RHID I2C Interface

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

/// @name Conveniences for rhid_command catch-all function

#define INLINE static inline __attribute__((always_inline))

typedef enum
{
    RHID_DIRECTION_RIGHT = 0,
    RHID_DIRECTION_LEFT = 1
} rhid_direction_t;

INLINE int rhid_clear(int fd) { return rhid_command(fd, RHID_COMMAND_CLEAR); }
INLINE int rhid_home(int fd) { return rhid_command(fd, RHID_COMMAND_HOME); }
INLINE int rhid_show_cursor(int fd, bool cursor) { return rhid_command(fd, cursor ? RHID_COMMAND_CURSOR : RHID_COMMAND_NOCURSOR); }
INLINE int rhid_blink(int fd, bool blink) { return rhid_command(fd, blink ? RHID_COMMAND_BLINK : RHID_COMMAND_NOBLINK); }
INLINE int rhid_display(int fd, bool display) { return rhid_command(fd, display ? RHID_COMMAND_DISPLAY : RHID_COMMAND_NODISPLAY); }
INLINE int rhid_scroll(int fd, rhid_direction_t direction) { return rhid_command(fd, direction ? RHID_COMMAND_SCROLLLEFT : RHID_COMMAND_SCROLLRIGHT); }
INLINE int rhid_text(int fd, rhid_direction_t direction) { return rhid_command(fd, direction ? RHID_COMMAND_TEXTLEFT : RHID_COMMAND_TEXTRIGHT); }
INLINE int rhid_autoscroll(int fd, bool autoscroll) { return rhid_command(fd, autoscroll ? RHID_COMMAND_AUTOSCROLL : RHID_COMMAND_NOAUTOSCROLL); }

/// @name Event loop system (guaranteed race free for single-threaded sytems)

typedef struct rhid_event *rhid_event_t;
rhid_event_t rhid_event_begin(int fd);
int rhid_event(rhid_event_t events);
int rhid_event_end(rhid_event_t events);

__END_DECLS

#endif
