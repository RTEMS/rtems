/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>

extern void bsp_spurious_initialize(void);

void bsp_predriver_hook(void)
{
  /* on CPU32: do something about vectors!!! */

  bsp_spurious_initialize();
}
