
#include <librhid/proto.h>
#include <librhid/hardware.h>

int proto_get_byte(int fd, uint8_t reg)
{
    uint8_t val = 0;
    ssize_t rv = proto_get_stream(fd, reg, &val, 1);
    
    if (rv > 0)
        return val;
    else
        return -1;
}

int proto_set_byte(int fd, uint8_t reg, uint8_t value)
{
    return proto_set_stream(fd, reg, &value, 1);
}

ssize_t proto_get_stream(int fd, uint8_t reg, void *buf, size_t len)
{
    ssize_t rv = write(fd, &reg, 1);
    if (rv != 1)
        return rv;

    return read(fd, buf, len);
}

ssize_t proto_set_stream(int fd, uint8_t reg, const void *buf, size_t len)
{
    uint8_t *obuf = malloc(len + 1);
    if (!obuf)
        return -1;
    obuf[0] = reg;
    memcpy(obuf + 1, buf, len);

    ssize_t rv = write(fd, obuf, len + 1);
    free(obuf);
    usleep(5000);
    return rv;
}

