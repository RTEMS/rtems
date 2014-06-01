/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>

extern void Spurious_Initialize(void);

/*
 *  Call Spurious_Initialize in bsp_predriver_hook because
 *  bsp_predriver_hook is call after the _ISR_Vector_Table allocation
 */
void bsp_predriver_hook(void)
{
  Spurious_Initialize();
}
