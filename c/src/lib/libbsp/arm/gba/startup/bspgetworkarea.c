/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <stdint.h>

extern int _end;
extern int __heap_limit;

void bsp_work_area_initialize(void)
{
  void *area_start = (void *)&_end;
  uintptr_t area_size  = (uintptr_t)&__heap_limit - (uintptr_t)&_end;

  bsp_work_area_initialize_default( area_start, area_size );
}

