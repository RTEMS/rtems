/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreMemory
 *
 * @brief This source file contains the i386/pc386 implementation of
 *   _Memory_Get().
 */

/*
 * Copyright (C) 2019 embedded brains GmbH (http://www.embedded-brains.de)
 *
 * Copyright (C) 1989, 2017 On-Line Applications Research Corporation (OAR)
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

/* #define BSP_GET_WORK_AREA_DEBUG */
#include <bsp.h>
#include <bsp/bootcard.h>

#include <rtems/sysinit.h>

#ifdef BSP_GET_WORK_AREA_DEBUG
  #include <rtems/bspIo.h>
#endif

/*
 *  These are provided by the linkcmds.
 */
extern char   WorkAreaBase[];
extern char   HeapSize[];
extern char   RamSize[];

/* rudimentary multiboot info */
struct multiboot_info {
  uint32_t  flags;       /* start.S only raises flags for items actually */
                         /* saved; this allows us to check for the size  */
                         /* of the data structure.                       */
  uint32_t  mem_lower;  /* avail kB in lower memory */
  uint32_t  mem_upper;  /* avail kB in lower memory */
  /* ... (unimplemented) */
};

extern struct multiboot_info _boot_multiboot_info;

/*
 *  This is the first address of the memory we can use for the RTEMS
 *  Work Area.
 */ 
static uintptr_t rtemsWorkAreaStart;

/*
 * Board's memory size easily be overridden by application.
 */
uint32_t bsp_mem_size = 0;

static void bsp_size_memory(void)
{
  uintptr_t topAddr;

  /* Set the value of start of free memory. */
  rtemsWorkAreaStart = (uint32_t)WorkAreaBase;

  /* Align the RTEMS Work Area at beginning of free memory. */
  if (rtemsWorkAreaStart & (CPU_ALIGNMENT - 1))  /* not aligned => align it */
    rtemsWorkAreaStart = (rtemsWorkAreaStart+CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

  /* The memory detection algorithm is very crude; try
   * to use multiboot info, if possible (set from start.S)
   */
  if ( ((uintptr_t)RamSize == (uintptr_t) 0xFFFFFFFF)  &&
       (_boot_multiboot_info.flags & 1) &&
       _boot_multiboot_info.mem_upper ) {
    topAddr = _boot_multiboot_info.mem_upper * 1024;
    #ifdef BSP_GET_WORK_AREA_DEBUG
      printk( "Multiboot info says we have 0x%08x\n", topAddr );
    #endif
  } else if ( (uintptr_t) RamSize == (uintptr_t) 0xFFFFFFFF ) {
    uintptr_t lowest;
    uint32_t  val;
    int       i;

    /*
     * We have to dynamically size memory. Memory size can be anything
     * between no less than 2M and 2048M.  If we can write a value to
     * an address and read the same value back, then the memory is there.
     *
     * WARNING: This can detect memory which should be reserved for
     *          graphics controllers which share the CPU's RAM.
     */

    /* find the lowest 1M boundary to probe */
    lowest = ((rtemsWorkAreaStart + (1<<20)) >> 20) + 1;
    if ( lowest  < 2 )
      lowest = 2;

    for (i=2048; i>=lowest; i--) {
      topAddr = i*1024*1024 - 4;
      *(volatile uint32_t*)topAddr = topAddr;
    }

    for(i=lowest; i<=2048; i++) {
      topAddr = i*1024*1024 - 4;
      val =  *(volatile uint32_t*)topAddr;
      if (val != topAddr) {
        break;
      }
    }

    topAddr = (i-1)*1024*1024;
    #ifdef BSP_GET_WORK_AREA_DEBUG
      printk( "Dynamically sized to 0x%08x\n", topAddr );
    #endif
  } else {
    topAddr = (uintptr_t) RamSize;
    #ifdef BSP_GET_WORK_AREA_DEBUG
      printk( "hardcoded to 0x%08x\n", topAddr );
    #endif
  }

  bsp_mem_size = topAddr;
}

static Memory_Area _Memory_Areas[ 1 ];

static void bsp_memory_initialize( void )
{
  /*
   *  We need to determine how much memory there is in the system.
   */
  bsp_size_memory();

  _Memory_Initialize_by_size(
    &_Memory_Areas[0],
    (void *) rtemsWorkAreaStart,
    (uintptr_t) bsp_mem_size - (uintptr_t) rtemsWorkAreaStart
  );
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
