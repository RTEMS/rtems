/* SPDX-License-Identifier: GPL-2.0+-with-RTEMS-exception */

/*
 * Cirrus EP7312 Intererrupt handler
 */

/*
 * Copyright (c) 2010 embedded brains GmbH & Co. KG
 *
 * Copyright (c) 2002 by Jay Monkman <jtm@smoothsmoothie.com>
 *
 * Copyright (c) 2002 by Charlie Steader <charlies@poliac.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
*/

#include <rtems/score/armv4.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>

#include <ep7312.h>

void edb7312_interrupt_dispatch(rtems_vector_number vector)
{
  bsp_interrupt_handler_dispatch(vector);
}

rtems_status_code bsp_interrupt_get_attributes(
  rtems_vector_number         vector,
  rtems_interrupt_attributes *attributes
)
{
  (void) vector;
  (void) attributes;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_is_pending(
  rtems_vector_number vector,
  bool               *pending
)
{
  (void) vector;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(pending != NULL);
  *pending = false;
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_raise(rtems_vector_number vector)
{
  (void) vector;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_clear(rtems_vector_number vector)
{
  (void) vector;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_vector_is_enabled(
  rtems_vector_number vector,
  bool               *enabled
)
{
  (void) vector;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(enabled != NULL);
  *enabled = false;
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
    bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

    if(vector >= BSP_EXTFIQ && vector <= BSP_SSEOTI)
    {
        /* interrupt managed by INTMR1 and INTSR1 */
        *EP7312_INTMR1 |= (1 << vector);
    }
    else if(vector >= BSP_KBDINT && vector <= BSP_SS2TX)
    {
        /* interrupt managed by INTMR2 and INTSR2 */
        *EP7312_INTMR2 |= (1 << (vector - 16));
    }
    else if(vector >= BSP_UTXINT2 && vector <= BSP_URXINT2)
    {
        /* interrupt managed by INTMR2 and INTSR2 */
        *EP7312_INTMR2 |= (1 << (vector - 7));
    }
    else if(vector == BSP_DAIINT)
    {
        /* interrupt managed by INTMR3 and INTSR3 */
        *EP7312_INTMR3 |= (1 << (vector - 21));
    }

    return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
    bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

    if(vector >= BSP_EXTFIQ && vector <= BSP_SSEOTI)
    {
        /* interrupt managed by INTMR1 and INTSR1 */
        *EP7312_INTMR1 &= ~(1 << vector);
    }
    else if(vector >= BSP_KBDINT && vector <= BSP_SS2TX)
    {
        /* interrupt managed by INTMR2 and INTSR2 */
        *EP7312_INTMR2 &= ~(1 << (vector - 16));
    }
    else if(vector >= BSP_UTXINT2 && vector <= BSP_URXINT2)
    {
        /* interrupt managed by INTMR2 and INTSR2 */
        *EP7312_INTMR2 &= ~(1 << (vector - 7));
    }
    else if(vector == BSP_DAIINT)
    {
        /* interrupt managed by INTMR3 and INTSR3 */
        *EP7312_INTMR3 &= ~(1 << (vector - 21));
    }

    return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_set_priority(
  rtems_vector_number vector,
  uint32_t priority
)
{
  (void) vector;
  (void) priority;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_get_priority(
  rtems_vector_number vector,
  uint32_t *priority
)
{
  (void) vector;
  (void) priority;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(priority != NULL);
  return RTEMS_UNSATISFIED;
}

void bsp_interrupt_facility_initialize(void)
{
  uint32_t int_stat = 0;

  /* mask all interrupts */
  *EP7312_INTMR1 = 0x0;
  *EP7312_INTMR2 = 0x0;
  *EP7312_INTMR3 = 0x0;
  
  /* clear all pending interrupt status' */
  int_stat = *EP7312_INTSR1;
  if(int_stat & EP7312_INTR1_EXTFIQ)
  {
  }
  if(int_stat & EP7312_INTR1_BLINT)
  {
      *EP7312_BLEOI = 0xFFFFFFFF;
  }
  if(int_stat & EP7312_INTR1_WEINT)
  {
      *EP7312_TEOI = 0xFFFFFFFF;
  }
  if(int_stat & EP7312_INTR1_MCINT)
  {
  }
  if(int_stat & EP7312_INTR1_CSINT)
  {
      *EP7312_COEOI = 0xFFFFFFFF;
  }
  if(int_stat & EP7312_INTR1_EINT1)
  {
  }
  if(int_stat & EP7312_INTR1_EINT2)
  {
  }
  if(int_stat & EP7312_INTR1_EINT3)
  {
  }
  if(int_stat & EP7312_INTR1_TC1OI)
  {
      *EP7312_TC1EOI = 0xFFFFFFFF;
  }
  if(int_stat & EP7312_INTR1_TC2OI)
  {
      *EP7312_TC2EOI = 0xFFFFFFFF;
  }
  if(int_stat & EP7312_INTR1_RTCMI)
  {
      *EP7312_RTCEOI = 0xFFFFFFFF;
  }
  if(int_stat & EP7312_INTR1_TINT)
  {
      *EP7312_TEOI = 0xFFFFFFFF;
  }
  if(int_stat & EP7312_INTR1_URXINT1)
  {
  }
  if(int_stat & EP7312_INTR1_UTXINT1)
  {
  }
  if(int_stat & EP7312_INTR1_UMSINT)
  {
      *EP7312_UMSEOI = 0xFFFFFFFF;
  }
  if(int_stat & EP7312_INTR1_SSEOTI)
  {
      *EP7312_SYNCIO;
  }
  int_stat = *EP7312_INTSR1;
  
  int_stat = *EP7312_INTSR2;
  if(int_stat & EP7312_INTR2_KBDINT)
  {
      *EP7312_KBDEOI = 0xFFFFFFFF;
  }
  if(int_stat & EP7312_INTR2_SS2RX)
  {
  }
  if(int_stat & EP7312_INTR2_SS2TX)
  {
  }
  if(int_stat & EP7312_INTR2_URXINT2)
  {
  }
  if(int_stat & EP7312_INTR2_UTXINT2)
  {
  }
  int_stat = *EP7312_INTSR2;
  
  int_stat = *EP7312_INTSR3;
  if(int_stat & EP7312_INTR2_DAIINT)
  {
  }
  int_stat = *EP7312_INTSR3;

  _CPU_ISR_install_vector(ARM_EXCEPTION_IRQ, _ARMV4_Exception_interrupt, NULL);
}
