
#include <librhid/hardware.h>
#include <librhid/proto.h>
#include <librhid/rhid.h>
#include <semaphore.h>

#ifdef XREF
#undef XREF
#endif

int rhid_open(int i2c_addr)
{
    int rv = 0;
    if ((rv = gpio_open(4)))
        return rv;

    if ((rv = gpio_direction(4, GPIO_INPUT)))
    {
        gpio_close(4);
        return rv;
    }

    if ((rv = gpio_open(22)))
    {
        gpio_close(4);
        return rv;
    }

    if ((rv = gpio_write(22, 0)))
    {
        gpio_close(4);
        gpio_close(22);
        return rv;
    }

    if ((rv = gpio_direction(22, GPIO_OUTPUT)))
    {
        gpio_close(4);
        gpio_close(22);
        return rv;
    }

    int fd = -1;
    if ((fd = i2c_open("/dev/i2c-1", i2c_addr)) < 0)
    {
        gpio_close(4);
        gpio_close(22);
        return -1;
    }

    if ((rv = rhid_get_buttons(fd)) < 0)
    {
        rhid_close(fd);
        return rv;
    }

    return fd;
}

void rhid_close(int fd)
{
    gpio_close(4);
    gpio_close(22);
    close(fd);
}

int rhid_version(int fd, void *buf, size_t len);

int rhid_reset(int fd, int hard)
{
    if (hard)
    {
        gpio_write(22, 1);
        usleep(1000L);
        gpio_write(22, 0);
        return 0;
    }
    else
    {
        return proto_set_byte(fd, 0, 0x55);
    }
}

int rhid_get_backlight(int fd)
{
    return proto_get_byte(fd, 2);
}

int rhid_set_backlight(int fd, uint8_t level)
{
    return proto_set_byte(fd, 2, level);
}

int rhid_get_buttons(int fd)
{
    return proto_get_byte(fd, 3);
}

int rhid_set_cursor(int fd, uint8_t line, uint8_t character)
{
    union
    {
        uint8_t map;
        struct
        {
            uint8_t line:2;
            uint8_t character:6;
        } __attribute__((packed)) components;
    } cursor_charmap;
    
    cursor_charmap.components.line = line;
    cursor_charmap.components.character = character;
    
    return proto_set_byte(fd, 4, cursor_charmap.map);
}

int rhid_puts(int fd, const void *buf, size_t len)
{
    return proto_set_stream(fd, 5, buf, len);
}

int rhid_putchar(int fd, uint8_t ch)
{
    return proto_set_byte(fd, 5, ch);
}

int rhid_set_char(int fd, uint8_t ch, uint8_t bitmap[8]);

#if XREF
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
#endif

int rhid_command(int fd, rhid_command_t command)
{
    return proto_set_byte(fd, 3, command);
}

struct rhid_link
{
    int value;
    struct rhid_link *next;
};

struct rhid_event
{
    int fd;
    pthread_t listening_thread;
    sem_t event_queue_semaphore;
    struct rhid_link *event_queue;
};

void *rhid_event_loop(rhid_event_t events);

void rhid_link_destroy(struct rhid_link *link)
{
    if (link->next)
        rhid_link_destroy(link->next);
    free(link);
}

rhid_event_t rhid_event_begin(int fd)
{
    rhid_event_t events = malloc(sizeof(struct rhid_event));
    if (!events)
        return events;
    memset(events, 0, sizeof(struct rhid_event));
    
    events->fd = fd;
    sem_init(&(events->event_queue_semaphore), 0, 0);
    pthread_create(&(events->listening_thread), NULL, (void *(*)(void *))rhid_event_loop, events);
    return events;
}

int rhid_event(rhid_event_t events)
{
    sem_wait(&(events->event_queue_semaphore));
    
    if (events->event_queue)
    {
        struct rhid_link *link = events->event_queue;
        events->event_queue = link->next;
        link->next = NULL;
        
        int value = link->value;
        free(link);
        return value;
    }
    
    return -1;
}

int rhid_event_end(rhid_event_t events)
{
    pthread_cancel(events->listening_thread);
    sem_destroy(&(events->event_queue_semaphore));
    rhid_link_destroy(events->event_queue);
    
    free(events);
}

void *rhid_event_loop(rhid_event_t events)
{
    pthread_detach(pthread_self());
    
    for (;;)
    {
        int rv;
        if ((rv = rhid_get_buttons(events->fd)) > 0)
        {
            struct rhid_link *link = malloc(struct rhid_link);
            if (!link)
                continue;
            
            link->value = rv;
            link->next = NULL;
            
            struct rhid_link *end = events->event_queue;
            for (; end->next; end = end->next);
            end->next = link;
            
            sem_post(&(events->event_queue_semaphore));
        }
    }
    
    return NULL;
}
