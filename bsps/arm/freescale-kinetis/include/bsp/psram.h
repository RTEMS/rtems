#ifndef __PSRAM_H_
#define __PSRAM_H_

#include <stddef.h>

#define PSRAM_BASE_ADDRESS CS2_BASE
#define IS_PSRAM_ADDRESS(addr) ((unsigned int)addr >= PSRAM_BASE_ADDRESS)
#define BOTH_IN_PSRAM_RANGE(addr1, addr2) (IS_PSRAM_ADDRESS(addr1) && IS_PSRAM_ADDRESS(addr2))
#define EITHER_IN_PSRAM_RANGE(addr1, addr2) (IS_PSRAM_ADDRESS(addr1) || IS_PSRAM_ADDRESS(addr2))
#define IS_ODD_ADDRESS(addr) ((unsigned int)addr % 2)
#define BOTH_ODD_ADDRESS(addr1, addr2) (IS_ODD_ADDRESS(addr1) && IS_ODD_ADDRESS(addr2))
#define EITHER_ODD_ADDRESS(addr1, addr2 ) (IS_ODD_ADDRESS(addr1) || IS_ODD_ADDRESS(addr2))
#define GET_ALIGNED_ADDRESS(addr) ((unsigned short *)((unsigned int)addr & ~(0x1u)))
#define GET_ALIGNED_LENGTH(len) (1 + ((len - 1) | (sizeof(size_t) - 1)))

char *kinetis_memcpy(char *dst, const char *src, size_t len);
int kinetis_snprintf (char *str, size_t len, const char *fmt, ...);
char *kinetis_malloc(size_t len);
void kinetis_free(void *ptr);
void kinetis_psram_clear(void);

#endif
