#ifndef CRC32_H
#define CRC32_H

#include <zlib.h>
#include <cyg/crc/crc.h>

#undef crc32
#define crc32(val, s, len) cyg_crc32_accumulate(val, (unsigned char *)s, len)

#endif
