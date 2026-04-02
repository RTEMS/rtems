/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief BSP specific initialization support routines
 *
 */

/*
 * COPYRIGHT (c) 1989-2020.
 * On-Line Applications Research Corporation (OAR).
 * Cobham Gaisler AB.
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

#include <bsp.h>
#include <bsp/fdt.h>
#include <bsp/riscv-fdt.h>

#include <rtems/sysinit.h>

#include <libfdt.h>

/*
 *  These are provided by the linkcmds for ALL of the BSPs which use this file.
 */
extern char WorkAreaBase[];
extern char RamEnd[];

static Memory_Area _Memory_Areas[ 1 ];

static void bsp_memory_initialize( void )
{
  void *end;
#if defined(RISCV_RAM_REGION_BEGIN) && defined(RISCV_RAM_REGION_SIZE)
  uintptr_t start = RISCV_RAM_REGION_BEGIN;
  uintptr_t size = RISCV_RAM_REGION_SIZE;
  end = (void*) start + size;
#else
  const void *fdt = bsp_fdt_get();

  /* get end of memory from the "/memory" node in the fdt */
  end = riscv_fdt_get_end_of_memory(fdt);
  if (end == NULL) {
    /* fall back to linker symbol if "/memory" node not found or invalid */
    end = RamEnd;
  }
#endif
  _Memory_Initialize( &_Memory_Areas[ 0 ], WorkAreaBase, end );
}

RTEMS_SYSINIT_ITEM(
  bsp_memory_initialize,
  RTEMS_SYSINIT_MEMORY,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

static const Memory_Information _Memory_Information =
  MEMORY_INFORMATION_INITIALIZER( _Memory_Areas );

const Memory_Information *_Memory_Get( void )
{
  return &_Memory_Information;
}
