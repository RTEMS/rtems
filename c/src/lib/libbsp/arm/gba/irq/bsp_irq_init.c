/**
 *  @file bsp_irq_init.c
 *
 *  This file contains the implementation of rtems initialization
 *  related to interrupt handling.
 */
/*
 *  RTEMS GBA BSP
 *
 *  Copyright (c) 2004  Markku Puro <markku.puro@kopteri.net>
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <irq.h>
#include <bsp.h>
#include <gba_registers.h>


/**
 *  @brief BSP_rtems_irq_mngt_init BSP routine initialize irq registers
 *
 *  @param  None
 *  @return None
 */
void BSP_rtems_irq_mngt_init(void)
{
  long *vectorTable;
  int i;

  vectorTable = (long *) VECTOR_TABLE;

  /* Initialize the vector table contents with default handler */
  for (i=0; i<BSP_MAX_INT; i++) {
      *(vectorTable + i) = (long)(default_int_handler);
  }

  /* clear all interrupt status flags */
  GBA_REG_IF = 0xffff;
  /* disable all interrupts */
  GBA_REG_IE = 0;
  /* set master interrupt enable */
  GBA_REG_IME = 1;
}
