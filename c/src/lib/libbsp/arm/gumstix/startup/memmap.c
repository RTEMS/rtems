/*
 *  Gumstix Memory Map
 *  2008 By Yang Xi <hiyangxi@gmail.com>
 *  Copyright (c) 2004 by Cogent Computer Systems
 *  Written by Jay Monkman <jtm@lopingdog.com>
 */

#include <rtems.h>
#include <libcpu/mmu.h>

mmu_sect_map_t mem_map[] = {
/*  <phys addr>  <virt addr> <size> <flags> */
  {0x40000000, 0x40000000,  1216,   MMU_CACHE_NONE},  /*Map I/O*/
  {0xA0000000, 0x00000000,   1,   MMU_CACHE_NONE},  /*sram*/
  {0xA0000000, 0xA0000000,  64,   MMU_CACHE_WBACK}, /* SDRAM */
  {0x00000000, 0x00000000,   0,   0}                /* The end */
};
