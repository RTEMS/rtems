/*  vmeintr.c
 *
 *  VMEbus support routines for the DMV170.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>
#include <vmeintr.h>

/* PAGE
 *
 *  VME_interrupt_Disable
 *
 *  This routine disables vme interupts
 *
 *  Input parameters:
 *    mask - interupt mask
 *
 *  Output parameters:  NONE
 *
 *  Return values: NONE
 */

void VME_interrupt_Disable (
  VME_interrupt_Mask                mask                        /* IN  */
)
{
  volatile rtems_unsigned8  *VME_interrupt_enable;
  rtems_unsigned8  value;

#if 0
  VME_interrupt_enable = ACC_VIE;
#else
  VME_interrupt_enable = 0;
#endif
  value = *VME_interrupt_enable;

  value &= ~mask;  /* turn off interrupts for all levels in mask */

  *VME_interrupt_enable = value;
}

/* PAGE
 *
 *  VME_interrupt_Enable
 *
 *  This routine enables vme interupts
 *
 *  Input parameters:
 *    mask - interupt mask
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 */

void VME_interrupt_Enable (
  VME_interrupt_Mask                mask                        /* IN  */
)
{
  volatile rtems_unsigned8  *VME_interrupt_enable;
  rtems_unsigned8  value;

#if 0
  VME_interrupt_enable = ACC_VIE;
#else
  VME_interrupt_enable = 0;
#endif
  value = *VME_interrupt_enable;

  value |= mask;  /* turn on interrupts for all levels in mask */

  *VME_interrupt_enable = value;
}
