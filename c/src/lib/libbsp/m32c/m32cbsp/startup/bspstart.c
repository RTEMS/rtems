/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>

extern void sys_sbrk(int);

/*
 * This routine would usually do the bulk of the system initialization.
 * But if a BSP doesn't need to do anything, it can use this version.
 */

void bsp_start( void )
{
  /* Tell the simulator not to test for heap/stack collision */
  sys_sbrk(0);
}
