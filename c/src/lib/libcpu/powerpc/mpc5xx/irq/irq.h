/*
 * irq.h
 *
 *  This include file describe the data structure and the functions implemented
 *  by rtems to write interrupt handlers.
 *
 *
 *  MPC5xx port sponsored by Defence Research and Development Canada - Suffield
 *  Copyright (C) 2004, Real-Time Systems Inc. (querbach@realtime.bc.ca)
 *
 *  Derived from libbsp/powerpc/mbx8xx/irq/irq.h:
 *
 *  CopyRight (C) 1999 valette@crf.canon.fr
 *
 *  This code is heavilly inspired by the public specification of STREAM V2
 *  that can be found at :
 *
 *      <http://www.chorus.com/Documentation/index.html> by following
 *  the STREAM API Specification Document link.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _LIBCPU_IRQ_H
#define _LIBCPU_IRQ_H

#include <rtems/irq.h>

#define CPU_ASM_IRQ_VECTOR_BASE 0x0

#ifndef ASM

extern volatile unsigned int ppc_cached_irq_mask;

/*
 * Symblolic IRQ names and related definitions.
 */

  /*
   * Base vector for our USIU IRQ handlers.
   */
#define CPU_USIU_VECTOR_BASE	(CPU_ASM_IRQ_VECTOR_BASE)
  /*
   * USIU IRQ handler related definitions
   */
#define CPU_USIU_IRQ_COUNT    	(16) /* 16 reserved but in the future... */
#define CPU_USIU_IRQ_MIN_OFFSET	(0)
#define CPU_USIU_IRQ_MAX_OFFSET	(CPU_USIU_IRQ_MIN_OFFSET + CPU_USIU_IRQ_COUNT - 1)
  /*
   * UIMB IRQ handlers related definitions
   */
#define CPU_UIMB_IRQ_COUNT	(32 - 8) /* first 8 overlap USIU */
#define CPU_UIMB_IRQ_MIN_OFFSET	(CPU_USIU_IRQ_COUNT + CPU_USIU_VECTOR_BASE)
#define CPU_UIMB_IRQ_MAX_OFFSET	(CPU_UIMB_IRQ_MIN_OFFSET + CPU_UIMB_IRQ_COUNT - 1)
  /*
   * PowerPc exceptions handled as interrupt where a rtems managed interrupt
   * handler might be connected
   */
#define CPU_PROC_IRQ_COUNT	(1)
#define CPU_PROC_IRQ_MIN_OFFSET	(CPU_UIMB_IRQ_MAX_OFFSET + 1)
#define CPU_PROC_IRQ_MAX_OFFSET	(CPU_PROC_IRQ_MIN_OFFSET + CPU_PROC_IRQ_COUNT - 1)
  /*
   * Summary
   */
#define CPU_IRQ_COUNT		(CPU_PROC_IRQ_MAX_OFFSET + 1)
#define CPU_MIN_OFFSET		(CPU_USIU_IRQ_MIN_OFFSET)
#define CPU_MAX_OFFSET		(CPU_PROC_IRQ_MAX_OFFSET)
  /*
   * USIU IRQ symbolic name definitions.
   */
#define CPU_USIU_EXT_IRQ_0      (CPU_USIU_IRQ_MIN_OFFSET + 0)
#define CPU_USIU_INT_IRQ_0      (CPU_USIU_IRQ_MIN_OFFSET + 1)

#define CPU_USIU_EXT_IRQ_1      (CPU_USIU_IRQ_MIN_OFFSET + 2)
#define CPU_USIU_INT_IRQ_1      (CPU_USIU_IRQ_MIN_OFFSET + 3)

#define CPU_USIU_EXT_IRQ_2      (CPU_USIU_IRQ_MIN_OFFSET + 4)
#define CPU_USIU_INT_IRQ_2      (CPU_USIU_IRQ_MIN_OFFSET + 5)

#define CPU_USIU_EXT_IRQ_3      (CPU_USIU_IRQ_MIN_OFFSET + 6)
#define CPU_USIU_INT_IRQ_3      (CPU_USIU_IRQ_MIN_OFFSET + 7)

#define CPU_USIU_EXT_IRQ_4      (CPU_USIU_IRQ_MIN_OFFSET + 8)
#define CPU_USIU_INT_IRQ_4      (CPU_USIU_IRQ_MIN_OFFSET + 9)

#define CPU_USIU_EXT_IRQ_5      (CPU_USIU_IRQ_MIN_OFFSET + 10)
#define CPU_USIU_INT_IRQ_5      (CPU_USIU_IRQ_MIN_OFFSET + 11)

#define CPU_USIU_EXT_IRQ_6      (CPU_USIU_IRQ_MIN_OFFSET + 12)
#define CPU_USIU_INT_IRQ_6      (CPU_USIU_IRQ_MIN_OFFSET + 13)

#define CPU_USIU_EXT_IRQ_7      (CPU_USIU_IRQ_MIN_OFFSET + 14)
#define CPU_USIU_INT_IRQ_7      (CPU_USIU_IRQ_MIN_OFFSET + 15)

  /*
   * Symbolic names for UISU interrupt sources.
   */
#define CPU_PERIODIC_TIMER	(CPU_USIU_INT_IRQ_6)
#define CPU_UIMB_INTERRUPT	(CPU_USIU_INT_IRQ_7)

  /*
   * UIMB IRQ symbolic name definitions.  The first 8 sources are aliases to
   * the USIU interrupts of the same number, because they are detected in
   * the USIU pending register rather than the UIMB pending register.
   */
#define CPU_UIMB_IRQ_0 		(CPU_USIU_INT_IRQ_0)
#define CPU_UIMB_IRQ_1 		(CPU_USIU_INT_IRQ_1)
#define CPU_UIMB_IRQ_2 		(CPU_USIU_INT_IRQ_2)
#define CPU_UIMB_IRQ_3 		(CPU_USIU_INT_IRQ_3)
#define CPU_UIMB_IRQ_4 		(CPU_USIU_INT_IRQ_4)
#define CPU_UIMB_IRQ_5 		(CPU_USIU_INT_IRQ_5)
#define CPU_UIMB_IRQ_6 		(CPU_USIU_INT_IRQ_6)
#define CPU_UIMB_IRQ_7 		(CPU_USIU_INT_IRQ_7)

#define CPU_UIMB_IRQ_8         	(CPU_UIMB_IRQ_MIN_OFFSET+ 0)
#define CPU_UIMB_IRQ_9          (CPU_UIMB_IRQ_MIN_OFFSET+ 1)
#define CPU_UIMB_IRQ_10         (CPU_UIMB_IRQ_MIN_OFFSET+ 2)
#define CPU_UIMB_IRQ_11         (CPU_UIMB_IRQ_MIN_OFFSET+ 3)
#define CPU_UIMB_IRQ_12         (CPU_UIMB_IRQ_MIN_OFFSET+ 4)
#define CPU_UIMB_IRQ_13         (CPU_UIMB_IRQ_MIN_OFFSET+ 5)
#define CPU_UIMB_IRQ_14         (CPU_UIMB_IRQ_MIN_OFFSET+ 6)
#define CPU_UIMB_IRQ_15         (CPU_UIMB_IRQ_MIN_OFFSET+ 7)
#define CPU_UIMB_IRQ_16         (CPU_UIMB_IRQ_MIN_OFFSET+ 8)
#define CPU_UIMB_IRQ_17         (CPU_UIMB_IRQ_MIN_OFFSET+ 9)
#define CPU_UIMB_IRQ_18         (CPU_UIMB_IRQ_MIN_OFFSET+ 0)
#define CPU_UIMB_IRQ_19         (CPU_UIMB_IRQ_MIN_OFFSET+11)
#define CPU_UIMB_IRQ_20         (CPU_UIMB_IRQ_MIN_OFFSET+12)
#define CPU_UIMB_IRQ_21         (CPU_UIMB_IRQ_MIN_OFFSET+13)
#define CPU_UIMB_IRQ_22         (CPU_UIMB_IRQ_MIN_OFFSET+14)
#define CPU_UIMB_IRQ_23         (CPU_UIMB_IRQ_MIN_OFFSET+15)
#define CPU_UIMB_IRQ_24         (CPU_UIMB_IRQ_MIN_OFFSET+16)
#define CPU_UIMB_IRQ_25         (CPU_UIMB_IRQ_MIN_OFFSET+17)
#define CPU_UIMB_IRQ_26         (CPU_UIMB_IRQ_MIN_OFFSET+18)
#define CPU_UIMB_IRQ_27         (CPU_UIMB_IRQ_MIN_OFFSET+19)
#define CPU_UIMB_IRQ_28         (CPU_UIMB_IRQ_MIN_OFFSET+20)
#define CPU_UIMB_IRQ_29         (CPU_UIMB_IRQ_MIN_OFFSET+21)
#define CPU_UIMB_IRQ_30         (CPU_UIMB_IRQ_MIN_OFFSET+22)
#define CPU_UIMB_IRQ_31         (CPU_UIMB_IRQ_MIN_OFFSET+23)

  /*
   * Symbolic names for UIMB interrupt sources.
   */
#define CPU_IRQ_SCI		(CPU_UIMB_IRQ_5)

  /*
   * Processor exceptions handled as rtems IRQ symbolic name definitions.
   */
#define CPU_DECREMENTER		(CPU_PROC_IRQ_MIN_OFFSET)

/*
 * Convert an rtems_irq_number constant to an interrupt level
 * suitable for programming into an I/O device's interrupt level field.
 */
int CPU_irq_level_from_symbolic_name(const rtems_irq_number name);

/*-------------------------------------------------------------------------+
| Function Prototypes.
+--------------------------------------------------------------------------*/

extern void CPU_rtems_irq_mng_init(unsigned cpuId);

#endif

#endif
