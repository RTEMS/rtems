/*  vmeintr.c
 *
 *  VMEbus support routines for the DMV152.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>
#include <vmeintr.h>

/*PAGE
 *
 *  VME_interrupt_Disable
 *
 */

void VME_interrupt_Disable (
  VME_interrupt_Mask                mask                        /* IN  */
)
{
  volatile rtems_unsigned8  *VME_interrupt_enable;
  rtems_unsigned8  value;

  VME_interrupt_enable = ACC_VIE;
  value = *VME_interrupt_enable;

  value &= ~mask;  /* turn off interrupts for all levels in mask */

  *VME_interrupt_enable = value;
}

/*PAGE
 *
 *  VME_interrupt_Enable
 *
 */

void VME_interrupt_Enable (
  VME_interrupt_Mask                mask                        /* IN  */
)
{
  volatile rtems_unsigned8  *VME_interrupt_enable;
  rtems_unsigned8  value;

  VME_interrupt_enable = ACC_VIE;
  value = *VME_interrupt_enable;

  value |= mask;  /* turn on interrupts for all levels in mask */

  *VME_interrupt_enable = value;
}
