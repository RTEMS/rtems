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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_POWERPC_MBX8XX_IRQ_IRQ_H
#define LIBBSP_POWERPC_MBX8XX_IRQ_IRQ_H

#include <rtems/irq.h>

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

extern volatile unsigned int ppc_cached_irq_mask;

/*
 * Symblolic IRQ names and related definitions.
 */

  /*
   * SIU IRQ handler related definitions
   */
#define BSP_SIU_IRQ_NUMBER    		16 /* 16 reserved but in the future... */
#define BSP_SIU_IRQ_LOWEST_OFFSET	0
#define  BSP_SIU_IRQ_MAX_OFFSET	 	(BSP_SIU_IRQ_LOWEST_OFFSET + BSP_SIU_IRQ_NUMBER - 1)
  /*
   * CPM IRQ handlers related definitions
   * CAUTION : BSP_CPM_IRQ_LOWEST_OFFSET should be equal to OPENPIC_VEC_SOURCE
   */
#define BSP_CPM_IRQ_NUMBER		32
#define BSP_CPM_IRQ_LOWEST_OFFSET	(BSP_SIU_IRQ_NUMBER + BSP_SIU_IRQ_LOWEST_OFFSET)
#define BSP_CPM_IRQ_MAX_OFFSET	 	(BSP_CPM_IRQ_LOWEST_OFFSET + BSP_CPM_IRQ_NUMBER - 1)
  /*
   * PowerPc exceptions handled as interrupt where a rtems managed interrupt
   * handler might be connected
   */
#define BSP_PROCESSOR_IRQ_NUMBER	1
#define BSP_PROCESSOR_IRQ_LOWEST_OFFSET (BSP_CPM_IRQ_MAX_OFFSET + 1)
#define BSP_PROCESSOR_IRQ_MAX_OFFSET	(BSP_PROCESSOR_IRQ_LOWEST_OFFSET + BSP_PROCESSOR_IRQ_NUMBER - 1)
  /*
   * Summary
   */
#define BSP_IRQ_NUMBER		(BSP_PROCESSOR_IRQ_MAX_OFFSET + 1)
#define BSP_LOWEST_OFFSET	(BSP_SIU_IRQ_LOWEST_OFFSET)
#define BSP_MAX_OFFSET		(BSP_PROCESSOR_IRQ_MAX_OFFSET)
    /*
     * Some SIU IRQ symbolic name definition. Please note that
     * INT IRQ are defined but a single one will be used to
     * redirect all CPM interrupt.
     */
#define BSP_SIU_EXT_IRQ_0      	0
#define BSP_SIU_INT_IRQ_0       1

#define BSP_SIU_EXT_IRQ_1      	2
#define BSP_SIU_INT_IRQ_1       3

#define BSP_SIU_EXT_IRQ_2      	4
#define BSP_SIU_INT_IRQ_2       5

#define BSP_SIU_EXT_IRQ_3      	6
#define BSP_SIU_INT_IRQ_3       7

#define BSP_SIU_EXT_IRQ_4      	8
#define BSP_SIU_INT_IRQ_4       9

#define BSP_SIU_EXT_IRQ_5      	10
#define BSP_SIU_INT_IRQ_5       11

#define BSP_SIU_EXT_IRQ_6      	12
#define BSP_SIU_INT_IRQ_6       13

#define BSP_SIU_EXT_IRQ_7      	14
#define BSP_SIU_INT_IRQ_7       15
  /*
   * Symbolic name for CPM interrupt on SIU Internal level 2
   */
#define BSP_CPM_INTERRUPT	BSP_SIU_INT_IRQ_2
#define BSP_PERIODIC_TIMER	BSP_SIU_INT_IRQ_6
#define BSP_FAST_ETHERNET_CTRL	BSP_SIU_INT_IRQ_3
  /*
     * Some CPM IRQ symbolic name definition
     */
#define BSP_CPM_IRQ_ERROR		BSP_CPM_IRQ_LOWEST_OFFSET
#define BSP_CPM_IRQ_PARALLEL_IO_PC4	(BSP_CPM_IRQ_LOWEST_OFFSET + 1)
#define BSP_CPM_IRQ_PARALLEL_IO_PC5	(BSP_CPM_IRQ_LOWEST_OFFSET + 2)
#define BSP_CPM_IRQ_SMC2_OR_PIP		(BSP_CPM_IRQ_LOWEST_OFFSET + 3)
#define BSP_CPM_IRQ_SMC1		(BSP_CPM_IRQ_LOWEST_OFFSET + 4)
#define BSP_CPM_IRQ_SPI			(BSP_CPM_IRQ_LOWEST_OFFSET + 5)
#define BSP_CPM_IRQ_PARALLEL_IO_PC6	(BSP_CPM_IRQ_LOWEST_OFFSET + 6)
#define BSP_CPM_IRQ_TIMER_4		(BSP_CPM_IRQ_LOWEST_OFFSET + 7)

#define BSP_CPM_IRQ_PARALLEL_IO_PC7	(BSP_CPM_IRQ_LOWEST_OFFSET + 9)
#define BSP_CPM_IRQ_PARALLEL_IO_PC8	(BSP_CPM_IRQ_LOWEST_OFFSET + 10)
#define BSP_CPM_IRQ_PARALLEL_IO_PC9	(BSP_CPM_IRQ_LOWEST_OFFSET + 11)
#define BSP_CPM_IRQ_TIMER_3		(BSP_CPM_IRQ_LOWEST_OFFSET + 12)

#define BSP_CPM_IRQ_PARALLEL_IO_PC10	(BSP_CPM_IRQ_LOWEST_OFFSET + 14)
#define BSP_CPM_IRQ_PARALLEL_IO_PC11	(BSP_CPM_IRQ_LOWEST_OFFSET + 15)
#define BSP_CPM_I2C			(BSP_CPM_IRQ_LOWEST_OFFSET + 16)
#define BSP_CPM_RISC_TIMER_TABLE	(BSP_CPM_IRQ_LOWEST_OFFSET + 17)
#define BSP_CPM_IRQ_TIMER_2		(BSP_CPM_IRQ_LOWEST_OFFSET + 18)

#define BSP_CPM_IDMA2			(BSP_CPM_IRQ_LOWEST_OFFSET + 20)
#define BSP_CPM_IDMA1			(BSP_CPM_IRQ_LOWEST_OFFSET + 21)
#define BSP_CPM_SDMA_CHANNEL_BUS_ERR	(BSP_CPM_IRQ_LOWEST_OFFSET + 22)
#define BSP_CPM_IRQ_PARALLEL_IO_PC12	(BSP_CPM_IRQ_LOWEST_OFFSET + 23)
#define BSP_CPM_IRQ_PARALLEL_IO_PC13	(BSP_CPM_IRQ_LOWEST_OFFSET + 24)
#define BSP_CPM_IRQ_TIMER_1		(BSP_CPM_IRQ_LOWEST_OFFSET + 25)
#define BSP_CPM_IRQ_PARALLEL_IO_PC14	(BSP_CPM_IRQ_LOWEST_OFFSET + 26)
#define BSP_CPM_IRQ_SCC4		(BSP_CPM_IRQ_LOWEST_OFFSET + 27)
#define BSP_CPM_IRQ_SCC3		(BSP_CPM_IRQ_LOWEST_OFFSET + 28)
#define BSP_CPM_IRQ_SCC2		(BSP_CPM_IRQ_LOWEST_OFFSET + 29)
#define BSP_CPM_IRQ_SCC1		(BSP_CPM_IRQ_LOWEST_OFFSET + 30)
#define BSP_CPM_IRQ_PARALLEL_IO_PC15	(BSP_CPM_IRQ_LOWEST_OFFSET + 31)
    /*
     * Some Processor exception handled as rtems IRQ symbolic name definition
     */
#define BSP_DECREMENTER			BSP_PROCESSOR_IRQ_LOWEST_OFFSET

#define BSP_INTERRUPT_VECTOR_MIN BSP_LOWEST_OFFSET

#define BSP_INTERRUPT_VECTOR_MAX BSP_MAX_OFFSET

#define CPM_INTERRUPT

/*-------------------------------------------------------------------------+
| Function Prototypes.
+--------------------------------------------------------------------------*/
/*
 * ------------------------ PPC SIU Mngt Routines -------
 */

/*
 * function to disable a particular irq at 8259 level. After calling
 * this function, even if the device asserts the interrupt line it will
 * not be propagated further to the processor
 */
int BSP_irq_disable_at_siu        (const rtems_irq_number irqLine);
/*
 * function to enable a particular irq at 8259 level. After calling
 * this function, if the device asserts the interrupt line it will
 * be propagated further to the processor
 */
int BSP_irq_enable_at_siu		(const rtems_irq_number irqLine);
/*
 * function to acknoledge a particular irq at 8259 level. After calling
 * this function, if a device asserts an enabled interrupt line it will
 * be propagated further to the processor. Mainly usefull for people
 * writting raw handlers as this is automagically done for rtems managed
 * handlers.
 */
int BSP_irq_ack_at_siu              	(const rtems_irq_number irqLine);
/*
 * function to check if a particular irq is enabled at 8259 level. After calling
 */
int BSP_irq_enabled_at_siu     	(const rtems_irq_number irqLine);

extern void BSP_rtems_irq_mng_init(unsigned cpuId);

extern int BSP_irq_enabled_at_cpm(const rtems_irq_number irqLine);

#ifdef __cplusplus
}
#endif

#endif

#endif
