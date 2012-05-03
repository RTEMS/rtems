/*
 *  GP32 Memory Map
 *
 *  Copyright (c) 2004 by Cogent Computer Systems
 *  Written by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
#include <rtems.h>
#include <libcpu/mmu.h>

/* Remember, the ARM920 has 64 TLBs. If you have more 1MB sections than
 * that, you'll have TLB lookups, which could hurt performance.
 */
mmu_sect_map_t mem_map[] = {
/*  <phys addr>  <virt addr> <size> <flags> */
    {0x0c000000, 0x00000000,   1, MMU_CACHE_NONE},     /* SDRAM for vectors */
    {0x0c000000, 0x0c000000,   7, MMU_CACHE_WTHROUGH}, /* SDRAM W cache */
    {0x0c700000, 0x0c700000,   1, MMU_CACHE_NONE},     /* SDRAM W/O cache */
    {0x14000000, 0x14000000,   16, MMU_CACHE_NONE},    /* Internals Regs - */
    {0x15000000, 0x15000000,   16, MMU_CACHE_NONE},    /* Internal Regs - */
    {0x00000000, 0x00000000,   0,    0}                /* The end */
};
