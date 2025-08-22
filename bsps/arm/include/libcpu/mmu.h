/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief ARM MMU header file
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
