/*
 *  VMEbus support routines for the Generation I board.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>
#include <rtems/vmeintr.h>

/*PAGE
 *
 *  VME_interrupt_Disable
 *
 */

void VME_interrupt_Disable (
  VME_interrupt_Mask                mask                        /* IN  */
)
{
  volatile uint8_t  *VME_interrupt_enable;
  uint8_t           value;

  VME_interrupt_enable = 0;
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
  volatile uint8_t          *VME_interrupt_enable;
  uint8_t          value;

  VME_interrupt_enable = 0;
  value = *VME_interrupt_enable;
  value |= mask;  /* turn on interrupts for all levels in mask */
  *VME_interrupt_enable = value;
}
