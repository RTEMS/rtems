/*  vmeintr.c
 *
 *  VMEbus support routines for the DMV152.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
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
