#ifndef CRC32_H
#define CRC32_H

#include <zlib.h>

#undef crc32
#define crc32(val, s, len) (crc32_z(val ^ 0xffffffff, (unsigned char *)s, len) ^ 0xffffffff)

#endif
