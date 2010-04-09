/**
 *  @file irq.h
 *
 *  This include file describe the data structure and the functions implemented
 *  by rtems to write interrupt handlers.
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

#ifndef _IRQ_H_
#define _IRQ_H_


#ifdef __cplusplus
extern "C" {
#endif

/*
 * Include some preprocessor value also used by assember code
 */

#define VECTOR_TABLE  (&irq_vector_table[0])

#include <stdint.h>
#include <rtems/irq.h>
#include <rtems.h>

extern void default_int_handler(rtems_irq_hdl_param unused);

/*---------------------------------------------------------------------------*
 *  MACROS                                                                   *
 *---------------------------------------------------------------------------*/

#define ENABLE_IRQ()   GBA_REG_IME = 1;
#define DISABLE_IRQ()  GBA_REG_IME = 0;


/*-------------------------------------------------------------------------+
| Constants
+--------------------------------------------------------------------------*/

typedef enum {
  BSP_IRQ_VBLANK    = 0,
  BSP_IRQ_HBLANK    = 1,
  BSP_IRQ_VCOUNTER  = 2,
  BSP_IRQ_TIMER0    = 3,
  BSP_IRQ_TIMER1    = 4,
  BSP_IRQ_TIMER2    = 5,
  BSP_IRQ_TIMER3    = 6,
  BSP_IRQ_SERIAL    = 7,
  BSP_IRQ_DMA0      = 8,
  BSP_IRQ_DMA1      = 9,
  BSP_IRQ_DMA2      = 10,
  BSP_IRQ_DMA3      = 11,
  BSP_IRQ_KEY       = 12,
  BSP_IRQ_CART      = 13,
  BSP_IRQ_NA14      = 14,
  BSP_IRQ_NA15      = 15,
  BSP_MAX_INT       = 16  /**< BSP_MAX_INT <= _irq_max_vector in linkcmds */
} rtems_irq_symbolic_name;


extern void        _irq_max_vector;               /**< defined in lincmds    */
extern uint32_t    irq_vector_table[BSP_MAX_INT]; /**< allocated in linkcmds */

/*-------------------------------------------------------------------------+
| Function Prototypes.
+--------------------------------------------------------------------------*/
/*
 * ------------------------ RTEMS Single Irq Handler Mngt Routines ----------------
 */

/**
 * @brief function to initialize the interrupt for a specific BSP
 */
void BSP_rtems_irq_mngt_init();

#ifdef __cplusplus
}
#endif

#endif /* _IRQ_H_ */
