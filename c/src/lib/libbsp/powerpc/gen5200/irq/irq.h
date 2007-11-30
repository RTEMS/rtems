/*===============================================================*\
| Project: RTEMS generic MPC5200 BSP                              |
+-----------------------------------------------------------------+
| Partially based on the code references which are named below.   |
| Adaptions, modifications, enhancements and any recent parts of  |
| the code are:                                                   |
|                    Copyright (c) 2005                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains declarations for the irq controller handler  |
\*===============================================================*/
/***********************************************************************/
/*                                                                     */
/*   Module:       irq.h                                               */
/*   Date:         07/17/2003                                          */
/*   Purpose:      RTEMS MPC5x00 CPU interrupt header file             */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Description:  This include file describe the data structure and   */
/*                 the functions implemented by rtems to write         */
/*                 interrupt handlers.                                 */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Code                                                              */
/*   References:   MPC8260ads CPU interrupt header file                */
/*   Module:       irq.h                                               */
/*   Project:      RTEMS 4.6.0pre1 / MCF8260ads BSP                    */
/*   Version       1.1                                                 */
/*   Date:         10/10/2002                                          */
/*                                                                     */
/*   Author(s) / Copyright(s):                                         */
/*                                                                     */
/*   Copyright (C) 1999 valette@crf.canon.fr                           */
/*                                                                     */
/*   This code is heavilly inspired by the public specification of     */
/*   STREAM V2 that can be found at:                                   */
/*                                                                     */
/*   <http://www.chorus.com/Documentation/index.html> by following     */
/*   the STREAM API Specification Document link.                       */
/*                                                                     */
/*   Modified for mpc8260 by Andy Dachs <a.dachs@sstl.co.uk>           */
/*   Surrey Satellite Technology Limited                               */
/*   The interrupt handling on the mpc8260 seems quite different from  */
/*   the 860 (I don't know the 860 well).  Although some interrupts    */
/*   are routed via the CPM irq and some are direct to the SIU they    */
/*   all appear logically the same.Therefore I removed the distinction */
/*   between SIU and CPM interrupts.                                   */
/*                                                                     */
/*   The license and distribution terms for this file may be           */
/*   found in found in the file LICENSE in this distribution or at     */
/*   http://www.rtems.com/license/LICENSE.                        */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Partially based on the code references which are named above.     */
/*   Adaptions, modifications, enhancements and any recent parts of    */
/*   the code are under the right of                                   */
/*                                                                     */
/*         IPR Engineering, Dachauer Straße 38, D-80335 München        */
/*                        Copyright(C) 2003                            */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   IPR Engineering makes no representation or warranties with        */
/*   respect to the performance of this computer program, and          */
/*   specifically disclaims any responsibility for any damages,        */
/*   special or consequential, connected with the use of this program. */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Version history:  1.0                                             */
/*                                                                     */
/***********************************************************************/

#ifndef LIBBSP_POWERPC_MPC5200_IRQ_IRQ_H
#define LIBBSP_POWERPC_MPC5200_IRQ_IRQ_H

#define CHK_CE_SHADOW(_pmce)	((_pmce) & 0x00000001)
#define CHK_CSE_STICKY(_pmce)	(((_pmce) >> 10) & 0x00000001)
#define CHK_MSE_STICKY(_pmce)	(((_pmce) >> 21) & 0x00000001)
#define CHK_PSE_STICKY(_pmce)	(((_pmce) >> 29) & 0x00000001)
#define CLR_CSE_STICKY(_pmce)	((_pmce) |= (1 << 29 ))
#define CLR_MSE_STICKY(_pmce)	((_pmce) |= (1 << 21 ))
#define CLR_PSE_STICKY(_pmce)	((_pmce) |= (1 << 10 ))
#define CSE_SOURCE(_source)	(((_source) >> 8) & 0x00000003)
#define MSE_SOURCE(_source)	(((_source) >> 16) & 0x0000001F)
#define PSE_SOURCE(_source)	(((_source) >> 24) & 0x0000001F)

/*
 * Peripheral IRQ handlers related definitions
 */
#define BSP_PER_IRQ_NUMBER		22
#define BSP_PER_IRQ_LOWEST_OFFSET	0
#define BSP_PER_IRQ_MAX_OFFSET		\
	(BSP_PER_IRQ_LOWEST_OFFSET + BSP_PER_IRQ_NUMBER - 1) /* 21 */
/*
 * Main IRQ handlers related definitions
 */
#define BSP_MAIN_IRQ_NUMBER		17
#define BSP_MAIN_IRQ_LOWEST_OFFSET	BSP_PER_IRQ_MAX_OFFSET + 1 /* 22 */
#define BSP_MAIN_IRQ_MAX_OFFSET		\
	(BSP_MAIN_IRQ_LOWEST_OFFSET + BSP_MAIN_IRQ_NUMBER - 1) /* 38 */
/*
 * Critical IRQ handlers related definitions
 */
#define BSP_CRIT_IRQ_NUMBER		4
#define BSP_CRIT_IRQ_LOWEST_OFFSET	BSP_MAIN_IRQ_MAX_OFFSET + 1 /* 39 */
#define BSP_CRIT_IRQ_MAX_OFFSET		\
	(BSP_CRIT_IRQ_LOWEST_OFFSET + BSP_CRIT_IRQ_NUMBER - 1) /* 42 */
/*
 * Summary of SIU interrupts
 */
#define BSP_SIU_IRQ_NUMBER		BSP_CRIT_IRQ_MAX_OFFSET + 1 /* 43 */
#define BSP_SIU_IRQ_LOWEST_OFFSET	BSP_PER_IRQ_LOWEST_OFFSET /* 0 */
#define BSP_SIU_IRQ_MAX_OFFSET		BSP_CRIT_IRQ_MAX_OFFSET	 /* 42 */
/*
 * Processor IRQ handlers related definitions
 */
#define BSP_PROCESSOR_IRQ_NUMBER	3
#define BSP_PROCESSOR_IRQ_LOWEST_OFFSET	BSP_CRIT_IRQ_MAX_OFFSET + 1 /* 44  */
#define BSP_PROCESSOR_IRQ_MAX_OFFSET	\
        (BSP_PROCESSOR_IRQ_LOWEST_OFFSET + BSP_PROCESSOR_IRQ_NUMBER - 1) /* 46 */
/*
 * Summary
 */
#define BSP_IRQ_NUMBER		BSP_PROCESSOR_IRQ_MAX_OFFSET + 1 /* 47 */
#define BSP_LOWEST_OFFSET	BSP_PER_IRQ_LOWEST_OFFSET /* 0 */
#define BSP_MAX_OFFSET		BSP_PROCESSOR_IRQ_MAX_OFFSET /* 46 */

#ifndef ASM

/*
extern volatile unsigned int ppc_cached_irq_mask;
*/

/*
 * index table for the module specific handlers, a few entries are only placeholders
 */
typedef enum {
  BSP_SIU_IRQ_SMARTCOMM		= BSP_PER_IRQ_LOWEST_OFFSET + 0,
  BSP_SIU_IRQ_PSC1		= BSP_PER_IRQ_LOWEST_OFFSET + 1,
  BSP_SIU_IRQ_PSC2		= BSP_PER_IRQ_LOWEST_OFFSET + 2,
  BSP_SIU_IRQ_PSC3		= BSP_PER_IRQ_LOWEST_OFFSET + 3,
  BSP_SIU_IRQ_PSC6		= BSP_PER_IRQ_LOWEST_OFFSET + 4,
  BSP_SIU_IRQ_ETH		= BSP_PER_IRQ_LOWEST_OFFSET + 5,
  BSP_SIU_IRQ_USB		= BSP_PER_IRQ_LOWEST_OFFSET + 6,
  BSP_SIU_IRQ_ATA		= BSP_PER_IRQ_LOWEST_OFFSET + 7,
  BSP_SIU_IRQ_PCI_CRT		= BSP_PER_IRQ_LOWEST_OFFSET + 8,
  BSP_SIU_IRQ_PCI_SC_RX	        = BSP_PER_IRQ_LOWEST_OFFSET + 9,
  BSP_SIU_IRQ_PCI_SC_TX	        = BSP_PER_IRQ_LOWEST_OFFSET + 10,
  BSP_SIU_IRQ_PSC4		= BSP_PER_IRQ_LOWEST_OFFSET + 11,
  BSP_SIU_IRQ_PSC5		= BSP_PER_IRQ_LOWEST_OFFSET + 12,
  BSP_SIU_IRQ_SPI_MODF	        = BSP_PER_IRQ_LOWEST_OFFSET + 13,
  BSP_SIU_IRQ_SPI_SPIF	        = BSP_PER_IRQ_LOWEST_OFFSET + 14,
  BSP_SIU_IRQ_I2C1		= BSP_PER_IRQ_LOWEST_OFFSET + 15,
  BSP_SIU_IRQ_I2C2		= BSP_PER_IRQ_LOWEST_OFFSET + 16,
  BSP_SIU_IRQ_MSCAN1		= BSP_PER_IRQ_LOWEST_OFFSET + 17,
  BSP_SIU_IRQ_MSCAN2		= BSP_PER_IRQ_LOWEST_OFFSET + 18,
  BSP_SIU_IRQ_IR_RX		= BSP_PER_IRQ_LOWEST_OFFSET + 19,
  BSP_SIU_IRQ_IR_TX		= BSP_PER_IRQ_LOWEST_OFFSET + 20,
  BSP_SIU_IRQ_XLB_ARB		= BSP_PER_IRQ_LOWEST_OFFSET + 21,

  /* SL_TIMER1 -- handler entry only used in case of SMI */
  BSP_SIU_IRQ_SL_TIMER1	        = BSP_MAIN_IRQ_LOWEST_OFFSET + 0,
  BSP_SIU_IRQ_IRQ1		= BSP_MAIN_IRQ_LOWEST_OFFSET + 1,
  BSP_SIU_IRQ_IRQ2		= BSP_MAIN_IRQ_LOWEST_OFFSET + 2,
  BSP_SIU_IRQ_IRQ3		= BSP_MAIN_IRQ_LOWEST_OFFSET + 3,
  /* LO_INT --  handler entry never used (only placeholder) */
  BSP_SIU_IRQ_LO_INT		= BSP_MAIN_IRQ_LOWEST_OFFSET + 4,
  BSP_SIU_IRQ_RTC_PER		= BSP_MAIN_IRQ_LOWEST_OFFSET + 5,
  BSP_SIU_IRQ_RTC_STW		= BSP_MAIN_IRQ_LOWEST_OFFSET + 6,
  BSP_SIU_IRQ_GPIO_STD		= BSP_MAIN_IRQ_LOWEST_OFFSET + 7,
  BSP_SIU_IRQ_GPIO_WKUP		= BSP_MAIN_IRQ_LOWEST_OFFSET + 8,
  BSP_SIU_IRQ_TMR0		= BSP_MAIN_IRQ_LOWEST_OFFSET + 9,
  BSP_SIU_IRQ_TMR1		= BSP_MAIN_IRQ_LOWEST_OFFSET + 10,
  BSP_SIU_IRQ_TMR2		= BSP_MAIN_IRQ_LOWEST_OFFSET + 1,
  BSP_SIU_IRQ_TMR3		= BSP_MAIN_IRQ_LOWEST_OFFSET + 12,
  BSP_SIU_IRQ_TMR4		= BSP_MAIN_IRQ_LOWEST_OFFSET + 13,
  BSP_SIU_IRQ_TMR5		= BSP_MAIN_IRQ_LOWEST_OFFSET + 14,
  BSP_SIU_IRQ_TMR6		= BSP_MAIN_IRQ_LOWEST_OFFSET + 15,
  BSP_SIU_IRQ_TMR7		= BSP_MAIN_IRQ_LOWEST_OFFSET + 16,

  BSP_SIU_IRQ_IRQ0		= BSP_CRIT_IRQ_LOWEST_OFFSET + 0,
  BSP_SIU_IRQ_SL_TIMER0		= BSP_CRIT_IRQ_LOWEST_OFFSET + 1,
  /* HI_INT -- handler entry never used (only placeholder) */
  BSP_SIU_IRQ_HI_INT		= BSP_CRIT_IRQ_LOWEST_OFFSET + 2,
  BSP_SIU_IRQ_CSS_WKUP		= BSP_CRIT_IRQ_LOWEST_OFFSET + 3,

  BSP_DECREMENTER		= BSP_PROCESSOR_IRQ_LOWEST_OFFSET + 0,
  BSP_SYSMGMT			= BSP_PROCESSOR_IRQ_LOWEST_OFFSET + 1,
  BSP_EXT                 	= BSP_PROCESSOR_IRQ_LOWEST_OFFSET + 2
} rtems_irq_symbolic_name;

#define BSP_CRIT_IRQ_PRIO_LEVELS	  		  4
/*#define BSP_PERIODIC_TIMER                  BSP_DECREMENTER*/
#define BSP_PERIODIC_TIMER                    BSP_SIU_IRQ_TMR6
/*#define CPM_INTERRUPT*/


/*
 * Type definition for RTEMS managed interrupts
 */
typedef unsigned char  rtems_irq_prio;
struct 	__rtems_irq_connect_data__;	/* forward declaratiuon */

typedef unsigned int rtems_irq_number;
typedef void *rtems_irq_hdl_param;
typedef void (*rtems_irq_hdl)(rtems_irq_hdl_param);
typedef void (*rtems_irq_enable)(const struct __rtems_irq_connect_data__*);
typedef void (*rtems_irq_disable)(const struct __rtems_irq_connect_data__*);
typedef int  (*rtems_irq_is_enabled)(const struct __rtems_irq_connect_data__*);

typedef struct __rtems_irq_connect_data__ {
  /*
   * IRQ line
   */
  rtems_irq_number              name;
  /*
   * handler. See comment on handler properties below in function prototype.
   */
  rtems_irq_hdl                 hdl;
  /*
   * Handler handle to store private data
   */
  rtems_irq_hdl_param           handle;
  /*
   * function for enabling interrupts at device level (ONLY!).
   * The BSP code will automatically enable it at i8259s level.
   * RATIONALE : anyway such code has to exist in current driver code.
   * It is usually called immediately AFTER connecting the interrupt handler.
   * RTEMS may well need such a function when restoring normal interrupt
   * processing after a debug session.
   *
   */
  rtems_irq_enable            on;
  /*
   * function for disabling interrupts at device level (ONLY!).
   * The code will disable it at i8259s level. RATIONALE : anyway
   * such code has to exist for clean shutdown. It is usually called
   * BEFORE disconnecting the interrupt. RTEMS may well need such
   * a function when disabling normal interrupt processing for
   * a debug session. May well be a NOP function.
   */
  rtems_irq_disable             off;
  /*
   * function enabling to know what interrupt may currently occur
   * if someone manipulates the i8259s interrupt mask without care...
   */
  rtems_irq_is_enabled        isOn;

#ifdef BSP_SHARED_HANDLER_SUPPORT
  /*
   *  Set to -1 for vectors forced to have only 1 handler
   */
  void *next_handler;
#endif

} rtems_irq_connect_data;

typedef struct {
  /*
   * size of all the table fields (*Tbl) described below.
   */
  unsigned int	 		irqNb;
  /*
   * Default handler used when disconnecting interrupts.
   */
  rtems_irq_connect_data	defaultEntry;
  /*
   * Table containing initials/current value.
   */
  rtems_irq_connect_data*	irqHdlTbl;
  /*
   * actual value of BSP_PER_IRQ_LOWEST_OFFSET...
   */
  rtems_irq_symbolic_name	irqBase;
  /*
   * software priorities associated with interrupts.
   * if irqPrio  [i]  >  intrPrio  [j]  it  means  that
   * interrupt handler hdl connected for interrupt name i
   * will  not be interrupted by the handler connected for interrupt j
   * The interrupt source  will be physically masked at i8259 level.
   */
    rtems_irq_prio*		irqPrioTbl;
} rtems_irq_global_settings;



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
int BSP_irq_disable_at_siu(const rtems_irq_symbolic_name irqLine);
/*
 * function to enable a particular irq. After calling
 * this function, if the device asserts the interrupt line it will
 * be propagated further to the processor
 */
int BSP_irq_enable_at_siu(const rtems_irq_symbolic_name irqLine);
/*
 * function to acknowledge a particular irq. After calling
 * this function, if a device asserts an enabled interrupt line it will
 * be propagated further to the processor. Mainly useful for people
 * writing raw handlers as this is automagically done for rtems managed
 * handlers.
 */
int BSP_irq_ack_at_siu(const rtems_irq_symbolic_name irqLine);
/*
 * function to check ifl d  a particular irq is enabled. After calling
 */
int BSP_irq_enabled_at_siu(const rtems_irq_symbolic_name irqLine);



/*
 * ------------------------ RTEMS Single Irq Handler Mngt Routines ----------------
 */
/*
 * function to connect a particular irq handler. This hanlder will NOT be called
 * directly as the result of the corresponding interrupt. Instead, a RTEMS
 * irq prologue will be called that will :
 *
 *	1) save the C scratch registers,
 *	2) switch to a interrupt stack if the interrupt is not nested,
 *	4) modify them to disable the current interrupt at  SIU level (and may
 *	be others depending on software priorities)
 *	5) aknowledge the SIU',
 *	6) demask the processor,
 *	7) call the application handler
 *
 * As a result the hdl function provided
 *
 *	a) can perfectly be written is C,
 * 	b) may also well directly call the part of the RTEMS API that can be used
 *	from interrupt level,
 *	c) It only responsible for handling the jobs that need to be done at
 *	the device level including (aknowledging/re-enabling the interrupt at device,
 *	level, getting the data,...)
 *
 *	When returning from the function, the following will be performed by
 *	the RTEMS irq epilogue :
 *
 *	1) masks the interrupts again,
 *	2) restore the original SIU interrupt masks
 *	3) switch back on the orinal stack if needed,
 *	4) perform rescheduling when necessary,
 *	5) restore the C scratch registers...
 *	6) restore initial execution flow
 *
 */
int BSP_install_rtems_irq_handler(const rtems_irq_connect_data*);
/*
 * function to get the current RTEMS irq handler for ptr->name. It enables to
 * define hanlder chain...
 */
int BSP_get_current_rtems_irq_handler(rtems_irq_connect_data* ptr);
/*
 * function to get disconnect the RTEMS irq handler for ptr->name.
 * This function checks that the value given is the current one for safety reason.
 * The user can use the previous function to get it.
 */
int BSP_remove_rtems_irq_handler(const rtems_irq_connect_data*);

void BSP_rtems_irq_mng_init(unsigned cpuId);

int BSP_rtems_irq_mngt_set(rtems_irq_global_settings* config);

#endif

#endif
