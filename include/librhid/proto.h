
#ifndef _RHID_PROTO_H_
#define _RHID_PROTO_H_

#include <sys/cdefs.h>
#include <sys/types.h>
#include <stdint.h>
#include <inttypes.h>

__BEGIN_DECLS

int proto_get_byte(int fd, uint8_t reg);
int proto_set_byte(int fd, uint8_t reg, uint8_t value);
ssize_t proto_get_stream(int fd, uint8_t reg, void *buf, size_t len);
ssize_t proto_set_stream(int fd, uint8_t reg, const void *buf, size_t len);

__END_DECLS

#endif
