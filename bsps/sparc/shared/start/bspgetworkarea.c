/*
 *  This set of routines are the BSP specific initialization
 *  support routines.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>

#include <rtems/sysinit.h>

/* Tells us where to put the workspace in case remote debugger is present.  */
extern uintptr_t rdb_start;

static Memory_Area _Memory_Areas[ 1 ];

static const Memory_Information _Memory_Information =
  MEMORY_INFORMATION_INITIALIZER( _Memory_Areas );

static void bsp_memory_initialize( void )
{
  void      *begin;
  uintptr_t  size;

  begin = &end;
  size = rdb_start - (uintptr_t)begin;
  _Memory_Initialize_by_size( &_Memory_Areas[ 0 ], begin, size );
}

RTEMS_SYSINIT_ITEM(
  bsp_memory_initialize,
  RTEMS_SYSINIT_MEMORY,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

const Memory_Information *_Memory_Get( void )
{
  return &_Memory_Information;
}
