/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsX8664AMD64
 *
 * @brief Global Descriptor Table
 */

/*
 * Copyright (C) 2024 Matheus Pecoraro
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

#ifndef GDT_H
#define GDT_H

#define GDT_CODE_SEG_OFFSET 0x8
#define GDT_DATA_SEG_OFFSET 0x10

#ifndef ASM
#include <rtems/score/basedefs.h>

#include <stdint.h>

typedef struct {
    uint16_t limit_low;         /* Lower 16 bits of the limit */
    uint16_t base_low;          /* Lower 16 bits of the base */
    uint8_t  base_middle;       /* Next 8 bits of the base */
    uint8_t  access;            /* Access flags */
    uint8_t  gran_limit_middle; /* Granularity flags and upper 4 bits of the limit */
    uint8_t  base_high;         /* Last 8 bits of the base */
} RTEMS_PACKED gdt_entry;

typedef struct {
    uint16_t size;
    uint64_t addr;
} RTEMS_PACKED gdt_desc;

extern const gdt_desc amd64_gdt_descriptor;
#endif // ASM

#endif // GDT_H
