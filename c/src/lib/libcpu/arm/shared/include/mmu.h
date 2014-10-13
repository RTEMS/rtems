/*
 *  ARM MMU header file
 */

/*
 *  Copyright (c) 2004 by Cogent Computer Systems
 *  Written by Jay Monkman <jtm@lopingdog.com>
 */

#ifndef __LIBCPU_MMU_H__
#define __LIBCPU_MMU_H__

#include <stdint.h>

#define MMU_SECT_SIZE         0x100000

#define MMU_CACHE_NONE        0x0
#define MMU_CACHE_BUFFERED    0x1
#define MMU_CACHE_WTHROUGH    0x2
#define MMU_CACHE_WBACK       0x3

typedef struct {
    uint32_t paddr;
    uint32_t vaddr;
    uint32_t size;         /* in MB */
    uint8_t  cache_flags;
} mmu_sect_map_t;

void mmu_init(mmu_sect_map_t *map);
void mmu_set_cpu_async_mode(void);

#endif /* __MMU_H__ */
