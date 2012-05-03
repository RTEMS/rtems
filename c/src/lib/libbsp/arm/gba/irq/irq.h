/**
 *  @file irq.h
 *
 *  This include file describe the data structure and the functions implemented
 *  by rtems to write interrupt handlers.
 */
/*
 *  RTEMS GBA BSP
 *
 *  Copyright (c) 2010 embedded brains GmbH.
 *
 *  Copyright (c) 2004  Markku Puro <markku.puro@kopteri.net>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __asm__

#include <rtems.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>

#endif /* __asm__ */

#ifndef _IRQ_H_
#define _IRQ_H_

/*---------------------------------------------------------------------------*
 *  MACROS                                                                   *
 *---------------------------------------------------------------------------*/

#define ENABLE_IRQ()   GBA_REG_IME = 1;
#define DISABLE_IRQ()  GBA_REG_IME = 0;


/*-------------------------------------------------------------------------+
| Constants
+--------------------------------------------------------------------------*/

#define BSP_IRQ_VBLANK   0
#define BSP_IRQ_HBLANK   1
#define BSP_IRQ_VCOUNTER 2
#define BSP_IRQ_TIMER0   3
#define BSP_IRQ_TIMER1   4
#define BSP_IRQ_TIMER2   5
#define BSP_IRQ_TIMER3   6
#define BSP_IRQ_SERIAL   7
#define BSP_IRQ_DMA0     8
#define BSP_IRQ_DMA1     9
#define BSP_IRQ_DMA2     10
#define BSP_IRQ_DMA3     11
#define BSP_IRQ_KEY      12
#define BSP_IRQ_CART     13
#define BSP_IRQ_NA14     14
#define BSP_IRQ_NA15     15
#define BSP_MAX_INT      16

#define BSP_INTERRUPT_VECTOR_MIN 0

#define BSP_INTERRUPT_VECTOR_MAX (BSP_MAX_INT - 1)

#endif /* _IRQ_H_ */
