/*
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>

extern void Spurious_Initialize(void);

/*
 *  Call Spurious_Initialize in bsp_predriver_hook because
 *  bsp_predriver_hook is call after the _ISR_Vector_Table allocation
 */
void bsp_predriver_hook(void)
{
  Spurious_Initialize();
}
