/*
 *  CSB336 Memory Map
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
    {0x08200000, 0x00000000,   1,    MMU_CACHE_NONE},     /* Mirror of SDRAM */
    {0x00100000, 0x00100000,   1,    MMU_CACHE_NONE},     /* Bootstrap ROM */
    {0x00200000, 0x00200000,   1,    MMU_CACHE_NONE},     /* Internal Regs */
    {0x08000000, 0x08000000,  32,    MMU_CACHE_WTHROUGH}, /* SDRAM */
    {0x10000000, 0x10000000,   8,    MMU_CACHE_NONE},     /* CS0 - Flash */
    {0x12000000, 0x12000000,   1,    MMU_CACHE_NONE},     /* CS1 - enet */
    {0x13000000, 0x13000000,   1,    MMU_CACHE_NONE},     /* CS2 - */
    {0x14000000, 0x14000000,   1,    MMU_CACHE_NONE},     /* CS3 - */
    {0x15000000, 0x15000000,   1,    MMU_CACHE_NONE},     /* CS4 - */
    {0x16000000, 0x16000000,   1,    MMU_CACHE_NONE},     /* CS5 - */
    {0x50000000, 0x50000000,   1,    MMU_CACHE_NONE},     /* ARM Test Regs */
    {0x00000000, 0x00000000,   0,    0}                   /* The end */
};
