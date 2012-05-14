/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief interrupt definitions.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_MIPS_XXX_IRQ_H
#define LIBBSP_MIPS_XXX_IRQ_H

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

#define BSP_INTERRUPT_VECTOR_MIN    0
#define RM5231_MAXIMUM_VECTORS      (MIPS_INTERRUPT_BASE+8)
#define BSP_INTERRUPT_VECTOR_MAX    RM5231_MAXIMUM_VECTORS

/** @} */

#endif /* LIBBSP_MIPS_JMR3904_IRQ_H */
