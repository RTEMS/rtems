/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief jmr3904 interrupt definitions.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_MIPS_JMR3904_IRQ_H
#define LIBBSP_MIPS_JMR3904_IRQ_H

#ifndef ASM
  #include <rtems.h>
  #include <rtems/irq.h>
  #include <rtems/irq-extension.h>
  #include <rtems/score/mips.h>
#endif

/**
 * @addtogroup bsp_interrupt
 *
 * @{
 */

#define BSP_INTERRUPT_VECTOR_MIN 0

/*
 *  Interrupt Vector Numbers
 *
 *  NOTE: Numbers 0-15 directly map to levels on the IRC.
 *        Number 16 is "1xxxx" per p. 164 of the TX3904 manual.
 */

  #define TX3904_IRQ_INT1        MIPS_INTERRUPT_BASE+0
  #define TX3904_IRQ_INT2        MIPS_INTERRUPT_BASE+1
  #define TX3904_IRQ_INT3        MIPS_INTERRUPT_BASE+2
  #define TX3904_IRQ_INT4        MIPS_INTERRUPT_BASE+3
  #define TX3904_IRQ_INT5        MIPS_INTERRUPT_BASE+4
  #define TX3904_IRQ_INT6        MIPS_INTERRUPT_BASE+5
  #define TX3904_IRQ_INT7        MIPS_INTERRUPT_BASE+6
  #define TX3904_IRQ_DMAC3       MIPS_INTERRUPT_BASE+7
  #define TX3904_IRQ_DMAC2       MIPS_INTERRUPT_BASE+8
  #define TX3904_IRQ_DMAC1       MIPS_INTERRUPT_BASE+9
  #define TX3904_IRQ_DMAC0       MIPS_INTERRUPT_BASE+10
  #define TX3904_IRQ_SIO0        MIPS_INTERRUPT_BASE+11
  #define TX3904_IRQ_SIO1        MIPS_INTERRUPT_BASE+12
  #define TX3904_IRQ_TMR0        MIPS_INTERRUPT_BASE+13
  #define TX3904_IRQ_TMR1        MIPS_INTERRUPT_BASE+14
  #define TX3904_IRQ_TMR2        MIPS_INTERRUPT_BASE+15
  #define TX3904_IRQ_INT0        MIPS_INTERRUPT_BASE+16
  #define TX3904_IRQ_SOFTWARE_1  MIPS_INTERRUPT_BASE+17
  #define TX3904_IRQ_SOFTWARE_2  MIPS_INTERRUPT_BASE+18
  #define TX3904_MAXIMUM_VECTORS MIPS_INTERRUPT_BASE+19

#define BSP_INTERRUPT_VECTOR_MAX TX3904_MAXIMUM_VECTORS

/** @} */

#endif /* LIBBSP_MIPS_JMR3904_IRQ_H */
