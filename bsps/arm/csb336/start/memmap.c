/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief CSB336 Memory Map
 */

/*
 * Copyright (C) 2004 Cogent Computer Systems
 * Written by Jay Monkman <jtm@lopingdog.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <rtems.h>
#include <libcpu/mmu.h>

/* Remember, the ARM920 has 64 TLBs. If you have more 1MB sections than
 * that, you'll have TLB lookups, which could hurt performance.
 */
mmu_sect_map_t mem_map[] = {
/*  <phys addr>  <virt addr> <size> <flags> */
    {0x08200000, 0x00000000,   1,    MMU_CACHE_WBACK},     /* Mirror of SDRAM */
    {0x00100000, 0x00100000,   1,    MMU_CACHE_NONE},     /* Bootstrap ROM */
    {0x00200000, 0x00200000,   2,    MMU_CACHE_NONE},     /* Internal Regs + eSRAM */

    {0x08000000, 0x08000000,   1,    MMU_CACHE_NONE},     /* SDRAM */
    {0x08100000, 0x08100000,   1,    MMU_CACHE_WTHROUGH}, /* SDRAM */
    {0x08200000, 0x08200000,  30,    MMU_CACHE_WBACK},    /* SDRAM */

    {0x10000000, 0x10000000,   8,    MMU_CACHE_NONE},     /* CS0 - Flash */
    {0x12000000, 0x12000000,   1,    MMU_CACHE_NONE},     /* CS1 - enet */
    {0x13000000, 0x13000000,   1,    MMU_CACHE_NONE},     /* CS2 - */
    {0x14000000, 0x14000000,   1,    MMU_CACHE_NONE},     /* CS3 - */
    {0x15000000, 0x15000000,   1,    MMU_CACHE_NONE},     /* CS4 - */
    {0x16000000, 0x16000000,   1,    MMU_CACHE_NONE},     /* CS5 - */
    {0x50000000, 0x50000000,   1,    MMU_CACHE_NONE},     /* ARM Test Regs */
    {0x00000000, 0x00000000,   0,    0}                   /* The end */
};
