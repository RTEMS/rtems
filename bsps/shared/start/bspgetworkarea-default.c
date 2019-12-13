/**
 * @file
 *
 * This routine is an implementation of the _Memory_Get()
 * that can be used by all BSPs following linkcmds conventions
 * regarding heap, stack, and workspace allocation.
 */

/*
 * COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
 *
 * Copyright (c) 2011-2012 embedded brains GmbH.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>

#if defined(HAS_UBOOT) && !defined(BSP_DISABLE_UBOOT_WORK_AREA_CONFIG)
  #define USE_UBOOT
#endif

/*
 *  These are provided by the linkcmds for ALL of the BSPs which use this file.
 */
extern char WorkAreaBase[];

/*
 *  We may get the size information from U-Boot or the linker scripts.
 */
#ifdef USE_UBOOT
#include <bsp/u-boot.h>
#include <rtems/sysinit.h>

static Memory_Area _Memory_Areas[ 1 ];

static void bsp_memory_initialize( void )
{
  char *end;

  end = (char *) bsp_uboot_board_info.bi_memstart
    + bsp_uboot_board_info.bi_memsize;
  _Memory_Initialize( &_Memory_Areas[ 0 ], WorkAreaBase, end );
}

RTEMS_SYSINIT_ITEM(
  bsp_memory_initialize,
  RTEMS_SYSINIT_MEMORY,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
#else /* !USE_UBOOT */
extern char RamEnd[];

static Memory_Area _Memory_Areas[] = {
  MEMORY_INITIALIZER(WorkAreaBase, RamEnd)
};
#endif /* USE_UBOOT */

static const Memory_Information _Memory_Information =
  MEMORY_INFORMATION_INITIALIZER( _Memory_Areas );

const Memory_Information *_Memory_Get( void )
{
  return &_Memory_Information;
}
