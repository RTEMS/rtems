/* irq.h
 *
 *  This include file describe the data structure and the functions implemented
 *  by rtems to write interrupt handlers.
 *
 *  CopyRight (C) 1999 valette@crf.canon.fr
 *
 *  This code is heavilly inspired by the public specification of STREAM V2
 *  that can be found at :
 *
 *      <http://www.chorus.com/Documentation/index.html> by following
 *  the STREAM API Specification Document link.
 *
 *  Modified for mpc8260 by Andy Dachs <a.dachs@sstl.co.uk>
 *  Surrey Satellite Technology Limited
 *    The interrupt handling on the mpc8260 seems quite different from
 *    the 860 (I don't know the 860 well).  Although some interrupts
 *    are routed via the CPM irq and some are direct to the SIU they all
 *    appear logically the same.  Therefore I removed the distinction
 *    between SIU and CPM interrupts.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_POWERPC_IRQ_H
#define LIBBSP_POWERPC_IRQ_H
#include <rtems/irq.h>

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

/*
extern volatile unsigned int ppc_cached_irq_mask;
*/

/*
 * Symblolic IRQ names and related definitions.
 */

  /*
   * CPM IRQ handlers related definitions
   * CAUTION : BSP_CPM_IRQ_LOWEST_OFFSET should be equal to OPENPIC_VEC_SOURCE
   */
#define BSP_CPM_IRQ_NUMBER		(64)
#define BSP_CPM_IRQ_LOWEST_OFFSET	(0)
#define BSP_CPM_IRQ_MAX_OFFSET		(BSP_CPM_IRQ_LOWEST_OFFSET + BSP_CPM_IRQ_NUMBER - 1)
  /*
   * PowerPc exceptions handled as interrupt where a rtems managed interrupt
   * handler might be connected
   */
#define BSP_PROCESSOR_IRQ_NUMBER	(1)
#define BSP_PROCESSOR_IRQ_LOWEST_OFFSET (BSP_CPM_IRQ_MAX_OFFSET + 1)
#define BSP_PROCESSOR_IRQ_MAX_OFFSET	(BSP_PROCESSOR_IRQ_LOWEST_OFFSET + BSP_PROCESSOR_IRQ_NUMBER - 1)
  /*
   * Summary
   */
#define BSP_IRQ_NUMBER		 	(BSP_PROCESSOR_IRQ_MAX_OFFSET + 1)
#define BSP_LOWEST_OFFSET		(BSP_CPM_IRQ_LOWEST_OFFSET)
#define BSP_MAX_OFFSET			(BSP_PROCESSOR_IRQ_MAX_OFFSET)

  /*
   * Some SIU IRQ symbolic name definition. Please note that
   * INT IRQ are defined but a single one will be used to
   * redirect all CPM interrupt.
   *
   * On the mpc8260 all this seems to be transparent.  Although the
   * CPM, PIT and TMCNT interrupt may well be the only interrupts routed
   * to the SIU at the hardware level all of them appear as CPM interupts
   * to software apart from the registers for setting priority.
   *
   * The MPC8260 User Manual seems shot through with inconsistencies
   * about this whole area.
   */

  /*
   * Some CPM IRQ symbolic name definition
   */
#define BSP_CPM_IRQ_ERROR		(BSP_CPM_IRQ_LOWEST_OFFSET + 0)
#define BSP_CPM_IRQ_I2C			(BSP_CPM_IRQ_LOWEST_OFFSET + 1)
#define BSP_CPM_IRQ_SPI			(BSP_CPM_IRQ_LOWEST_OFFSET + 2)
#define BSP_CPM_IRQ_RISC_TIMERS		(BSP_CPM_IRQ_LOWEST_OFFSET + 3)
#define BSP_CPM_IRQ_SMC1		(BSP_CPM_IRQ_LOWEST_OFFSET + 4)
#define BSP_CPM_IRQ_SMC2		(BSP_CPM_IRQ_LOWEST_OFFSET + 5)
#define BSP_CPM_IRQ_IDMA1		(BSP_CPM_IRQ_LOWEST_OFFSET + 6)
#define BSP_CPM_IRQ_IDMA2		(BSP_CPM_IRQ_LOWEST_OFFSET + 7)
#define BSP_CPM_IRQ_IDMA3		(BSP_CPM_IRQ_LOWEST_OFFSET + 8)
#define BSP_CPM_IRQ_IDMA4		(BSP_CPM_IRQ_LOWEST_OFFSET + 9)
#define BSP_CPM_IRQ_SDMA		(BSP_CPM_IRQ_LOWEST_OFFSET + 10)

#define BSP_CPM_IRQ_TIMER_1		(BSP_CPM_IRQ_LOWEST_OFFSET + 12)
#define BSP_CPM_IRQ_TIMER_2		(BSP_CPM_IRQ_LOWEST_OFFSET + 13)
#define BSP_CPM_IRQ_TIMER_3		(BSP_CPM_IRQ_LOWEST_OFFSET + 14)
#define BSP_CPM_IRQ_TIMER_4		(BSP_CPM_IRQ_LOWEST_OFFSET + 15)
#define BSP_CPM_IRQ_TMCNT		(BSP_CPM_IRQ_LOWEST_OFFSET + 16)
#define BSP_CPM_IRQ_PIT			(BSP_CPM_IRQ_LOWEST_OFFSET + 17)

#define BSP_CPM_IRQ_IRQ1		(BSP_CPM_IRQ_LOWEST_OFFSET + 19)
#define BSP_CPM_IRQ_IRQ2		(BSP_CPM_IRQ_LOWEST_OFFSET + 20)
#define BSP_CPM_IRQ_IRQ3		(BSP_CPM_IRQ_LOWEST_OFFSET + 21)
#define BSP_CPM_IRQ_IRQ4		(BSP_CPM_IRQ_LOWEST_OFFSET + 22)
#define BSP_CPM_IRQ_IRQ5		(BSP_CPM_IRQ_LOWEST_OFFSET + 23)
#define BSP_CPM_IRQ_IRQ6		(BSP_CPM_IRQ_LOWEST_OFFSET + 24)
#define BSP_CPM_IRQ_IRQ7		(BSP_CPM_IRQ_LOWEST_OFFSET + 25)

#define BSP_CPM_IRQ_FCC1		(BSP_CPM_IRQ_LOWEST_OFFSET + 32)
#define BSP_CPM_IRQ_FCC2		(BSP_CPM_IRQ_LOWEST_OFFSET + 33)
#define BSP_CPM_IRQ_FCC3		(BSP_CPM_IRQ_LOWEST_OFFSET + 34)
#define BSP_CPM_IRQ_MCC1		(BSP_CPM_IRQ_LOWEST_OFFSET + 36)
#define BSP_CPM_IRQ_MCC2		(BSP_CPM_IRQ_LOWEST_OFFSET + 37)

#define BSP_CPM_IRQ_SCC1		(BSP_CPM_IRQ_LOWEST_OFFSET + 40)
#define BSP_CPM_IRQ_SCC2		(BSP_CPM_IRQ_LOWEST_OFFSET + 41)
#define BSP_CPM_IRQ_SCC3		(BSP_CPM_IRQ_LOWEST_OFFSET + 42)
#define BSP_CPM_IRQ_SCC4		(BSP_CPM_IRQ_LOWEST_OFFSET + 43)

#define BSP_CPM_IRQ_PC15		(BSP_CPM_IRQ_LOWEST_OFFSET + 48)
#define BSP_CPM_IRQ_PC14		(BSP_CPM_IRQ_LOWEST_OFFSET + 49)
#define BSP_CPM_IRQ_PC13		(BSP_CPM_IRQ_LOWEST_OFFSET + 50)
#define BSP_CPM_IRQ_PC12		(BSP_CPM_IRQ_LOWEST_OFFSET + 51)
#define BSP_CPM_IRQ_PC11		(BSP_CPM_IRQ_LOWEST_OFFSET + 52)
#define BSP_CPM_IRQ_PC10		(BSP_CPM_IRQ_LOWEST_OFFSET + 53)
#define BSP_CPM_IRQ_PC9			(BSP_CPM_IRQ_LOWEST_OFFSET + 54)
#define BSP_CPM_IRQ_PC8			(BSP_CPM_IRQ_LOWEST_OFFSET + 55)
#define BSP_CPM_IRQ_PC7			(BSP_CPM_IRQ_LOWEST_OFFSET + 56)
#define BSP_CPM_IRQ_PC6			(BSP_CPM_IRQ_LOWEST_OFFSET + 57)
#define BSP_CPM_IRQ_PC5			(BSP_CPM_IRQ_LOWEST_OFFSET + 58)
#define BSP_CPM_IRQ_PC4			(BSP_CPM_IRQ_LOWEST_OFFSET + 59)
#define BSP_CPM_IRQ_PC3			(BSP_CPM_IRQ_LOWEST_OFFSET + 60)
#define BSP_CPM_IRQ_PC2			(BSP_CPM_IRQ_LOWEST_OFFSET + 61)
#define BSP_CPM_IRQ_PC1			(BSP_CPM_IRQ_LOWEST_OFFSET + 62)
#define BSP_CPM_IRQ_PC0			(BSP_CPM_IRQ_LOWEST_OFFSET + 63)

    /*
     * Some Processor exception handled as rtems IRQ symbolic name definition
     */
#define BSP_DECREMENTER			(BSP_PROCESSOR_IRQ_LOWEST_OFFSET)
#define BSP_PERIODIC_TIMER		(BSP_DECREMENTER)

#define BSP_INTERRUPT_VECTOR_MIN BSP_LOWEST_OFFSET

#define BSP_INTERRUPT_VECTOR_MAX BSP_MAX_OFFSET

#define CPM_INTERRUPT

/*-------------------------------------------------------------------------+
| Function Prototypes.
+--------------------------------------------------------------------------*/
/*
 * ------------------------ PPC CPM Mngt Routines -------
 */

/*
 * function to disable a particular irq. After calling
 * this function, even if the device asserts the interrupt line it will
 * not be propagated further to the processor
 */
int BSP_irq_disable_at_cpm        (const rtems_irq_number irqLine);
/*
 * function to enable a particular irq. After calling
 * this function, if the device asserts the interrupt line it will
 * be propagated further to the processor
 */
int BSP_irq_enable_at_cpm		(const rtems_irq_number irqLine);
/*
 * function to acknoledge a particular irq. After calling
 * this function, if a device asserts an enabled interrupt line it will
 * be propagated further to the processor. Mainly usefull for people
 * writting raw handlers as this is automagically done for rtems managed
 * handlers.
 */
int BSP_irq_ack_at_cpm              	(const rtems_irq_number irqLine);
/*
 * function to check if a particular irq is enabled. After calling
 */
int BSP_irq_enabled_at_cpm     	(const rtems_irq_number irqLine);

extern void BSP_rtems_irq_mng_init(unsigned cpuId);

#ifdef __cplusplus
}
#endif

/* Now that we have defined some basics, include the generic support */
#include <bsp/irq-generic.h>

#endif

#endif
