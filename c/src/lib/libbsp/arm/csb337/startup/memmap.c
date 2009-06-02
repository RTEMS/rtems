/*
 *  CSB337 Memory Map
 *
 *  Copyright (c) 2004 by Cogent Computer Systems
 *  Written by Jay Monkman <jtm@lopingdog.com>
 *
 *  $Id$
 */
#include <rtems.h>
#include <libcpu/mmu.h>

#include <bspopts.h>

/* Remember, the ARM920 has 64 TLBs. If you have more 1MB sections than
 * that, you'll have TLB lookups, which could hurt performance.
 */
mmu_sect_map_t mem_map[] = {
/*  <phys addr>  <virt addr> <size> <flags> */
    {0x00200000, 0x00000000,   1,    MMU_CACHE_NONE},     /* SRAM */
    {0x00200000, 0x00200000,   1,    MMU_CACHE_NONE},     /* SRAM */
    {0x10000000, 0x10000000,   8,    MMU_CACHE_NONE},     /* FLASH */
#if csb637
    {0x20000000, 0x20000000,  64,    MMU_CACHE_WTHROUGH}, /* SDRAM */
#else /* CSB337 */
    {0x20000000, 0x20000000,  32,    MMU_CACHE_WTHROUGH}, /* SDRAM */
#endif
#if ENABLE_LCD
    {0x30000000, 0x30000000,   8,    MMU_CACHE_NONE},     /* video */
#endif
    {0x40000000, 0x40000000,   1,    MMU_CACHE_NONE},     /* Expansion CS0 */
    {0x50000000, 0x50000000,   1,    MMU_CACHE_NONE},     /* CF CE 1 */
    {0x60000000, 0x60000000,   1,    MMU_CACHE_NONE},     /* CF CE 1 */
    {0x70000000, 0x70000000,   1,    MMU_CACHE_NONE},     /* CF CE 2 */
    {0x80000000, 0x80000000,   1,    MMU_CACHE_NONE},     /* Expansion CS0 */
    {0xfff00000, 0xfff00000,   1,    MMU_CACHE_NONE},     /* Internal regs */
    {0x00000000, 0x00000000,   0,    0}                   /* The end */
};
