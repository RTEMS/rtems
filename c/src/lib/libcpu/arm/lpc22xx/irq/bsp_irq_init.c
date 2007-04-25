/*
 * Motorola LPC22XX Interrupt handler
 *
 * Copyright (c) 2004 by Jay Monkman <jtm@lopingdog.com>
 *      
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.com/license/LICENSE.
 *
 *
 *  $Id$
 */
#include <irq.h>
#include <bsp.h>
#include <lpc22xx.h>


extern void default_int_handler();

/* 
 * Interrupt system initialization. Disable interrupts, clear 
 * any that are pending.
 */
void BSP_rtems_irq_mngt_init()
{

    /* disable all interrupts */
  VICIntEnClr = 0xFFFFFFFF;

   
  /*
   * Set IRQHandler
   */
  IRQ_VECTOR_ADDR = 0xE59FF018;           /* LDR PC,[PC,#0x18] instruction */

  /*
   * Set FIQHandler
   */
  FIQ_VECTOR_ADDR = 0xE59FF018;           /* LDR PC,[PC,#0x18] instruction */

  /*
   * We does not need the next interrupt sources in the moment,
   * therefore jump to itself.
   */
  UNDEFINED_INSTRUCTION_VECTOR_ADDR = 0xEAFFFFFE;
  SOFTWARE_INTERRUPT_VECTOR_ADDR    = 0xEAFFFFFE;
  PREFETCH_ABORT_VECTOR_ADDR        = 0xEAFFFFFE;

  /*
   * In case we must find an ABORT error,
   * enable the next lines and set a breakpoint
   * in ABORTHandler.
   */  
#if 1
  DATA_ABORT_VECTOR_ADDR = 0xE59FF018;
#endif  

  /*
   * Init the Vectored Interrupt Controller (VIC)
   */
  VICProtection = 0;
  VICIntSelect = 0;
  

}

