/**
 *  @file irq.c
 *
 *  This file contains the implementation of the function described in irq.h.
 */
/*
 *  RTEMS GBA BSP
 *
 *  Copyright (c) 2010 embedded brains GmbH.
 *
 *  Copyright (c) 2002 by Jay Monkman <jtm@smoothsmoothie.com>
 *
 *  Copyright (c) 2002 by Charlie Steader <charlies@poliac.com>
 *
 *  Copyright (c) 2004 by Markku Puro <markku.puro@kopteri.net>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>

#include <gba_registers.h>

void bsp_interrupt_dispatch(void)
{
  unsigned reg_ie = GBA_REG_IE;
  unsigned reg_if = GBA_REG_IF & reg_ie;
  rtems_vector_number vector = 31 - __builtin_clz(reg_if);

  bsp_interrupt_handler_dispatch(vector);

  GBA_REG_IF = 1 << vector;
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  GBA_REG_IE |= 1 << vector;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  GBA_REG_IE &= ~(1 << vector);

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
  /* clear all interrupt status flags */
  GBA_REG_IF = 0xffff;
  /* disable all interrupts */
  GBA_REG_IE = 0;
  /* set master interrupt enable */
  GBA_REG_IME = 1;

  /* Exception handler is already present in the ROM BIOS */

  return RTEMS_SUCCESSFUL;
}
