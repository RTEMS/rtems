/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>

#include <libcpu/powerpc-utility.h>

#include <rtems/sysinit.h>

LINKER_SYMBOL(__rtems_end)

static Memory_Area _Memory_Areas[1];

static const Memory_Information _Memory_Information =
  MEMORY_INFORMATION_INITIALIZER(_Memory_Areas);

static void bsp_memory_initialize(void)
{
  uintptr_t size;
  uintptr_t begin;

  begin = (uintptr_t)__rtems_end;
  size = (uintptr_t)BSP_mem_size - begin;

  _Memory_Initialize_by_size(&_Memory_Areas[0], (void *) begin, size);
}

RTEMS_SYSINIT_ITEM(
  bsp_memory_initialize,
  RTEMS_SYSINIT_MEMORY,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

const Memory_Information *_Memory_Get(void)
{
  return &_Memory_Information;
}
