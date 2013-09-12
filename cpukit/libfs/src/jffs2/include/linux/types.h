#ifndef __LINUX_TYPES_H__
#define __LINUX_TYPES_H__

#include <sys/types.h>
#include <stdint.h>

#define loff_t off_t

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef uint8_t __u8;
typedef uint16_t __u16;
typedef uint32_t __u32;

#define kvec iovec

struct qstr {
	const char *name;
	size_t len;
};

#endif /* __LINUX_TYPES_H__ */

