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
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef LIBBSP_POWERPC_ETH_COMM_IRQ_IRQ_H
#define LIBBSP_POWERPC_ETH_COMM_IRQ_IRQ_H


#define BSP_ASM_IRQ_VECTOR_BASE 0x0

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

extern volatile unsigned int ppc_cached_irq_mask;

/*
 * Symblolic IRQ names and related definitions.
 */

typedef enum {
  /* Base vector for our SIU IRQ handlers. */
  BSP_SIU_VECTOR_BASE	=	BSP_ASM_IRQ_VECTOR_BASE,
  /*
   * SIU IRQ handler related definitions
   */
  BSP_SIU_IRQ_NUMBER    	= 	16, /* 16 reserved but in the future... */
  BSP_SIU_IRQ_LOWEST_OFFSET	= 	0,
  BSP_SIU_IRQ_MAX_OFFSET	= 	BSP_SIU_IRQ_LOWEST_OFFSET + BSP_SIU_IRQ_NUMBER - 1,
  /*
   * CPM IRQ handlers related definitions
   * CAUTION : BSP_CPM_IRQ_LOWEST_OFFSET should be equal to OPENPIC_VEC_SOURCE
   */
  BSP_CPM_IRQ_NUMBER		=	32,
  BSP_CPM_IRQ_LOWEST_OFFSET	=	BSP_SIU_IRQ_NUMBER + BSP_SIU_VECTOR_BASE,
  BSP_CPM_IRQ_MAX_OFFSET	= 	BSP_CPM_IRQ_LOWEST_OFFSET + BSP_CPM_IRQ_NUMBER - 1,
  /*
   * PowerPc exceptions handled as interrupt where a rtems managed interrupt
   * handler might be connected
   */
  BSP_PROCESSOR_IRQ_NUMBER	=	1,
  BSP_PROCESSOR_IRQ_LOWEST_OFFSET = 	BSP_CPM_IRQ_MAX_OFFSET + 1,
  BSP_PROCESSOR_IRQ_MAX_OFFSET	=	BSP_PROCESSOR_IRQ_LOWEST_OFFSET + BSP_PROCESSOR_IRQ_NUMBER - 1,
  /*
   * Summary
   */
  BSP_IRQ_NUMBER		= 	BSP_PROCESSOR_IRQ_MAX_OFFSET + 1,
  BSP_LOWEST_OFFSET		=	BSP_SIU_IRQ_LOWEST_OFFSET,
  BSP_MAX_OFFSET		=	BSP_PROCESSOR_IRQ_MAX_OFFSET,
    /*
     * Some SIU IRQ symbolic name definition. Please note that
     * INT IRQ are defined but a single one will be used to
     * redirect all CPM interrupt.
     */	       
  BSP_SIU_EXT_IRQ_0      	= 	0,
  BSP_SIU_INT_IRQ_0         	= 	1,

  BSP_SIU_EXT_IRQ_1      	= 	2,
  BSP_SIU_INT_IRQ_1         	= 	3,

  BSP_SIU_EXT_IRQ_2      	= 	4,
  BSP_SIU_INT_IRQ_2         	= 	5,
  
  BSP_SIU_EXT_IRQ_3      	= 	6,
  BSP_SIU_INT_IRQ_3         	= 	7,
  
  BSP_SIU_EXT_IRQ_4      	= 	8,
  BSP_SIU_INT_IRQ_4         	= 	9,

  BSP_SIU_EXT_IRQ_5      	= 	10,
  BSP_SIU_INT_IRQ_5         	= 	11,
  
  BSP_SIU_EXT_IRQ_6      	= 	12,
  BSP_SIU_INT_IRQ_6         	= 	13,
  
  BSP_SIU_EXT_IRQ_7      	= 	14,
  BSP_SIU_INT_IRQ_7         	= 	15,
  /*
   * Symbolic name for CPM interrupt on SIU Internal level 2
   */
  BSP_CPM_INTERRUPT		=	BSP_SIU_INT_IRQ_2,
  BSP_PERIODIC_TIMER		=	BSP_SIU_INT_IRQ_6,
  BSP_FAST_ETHERNET_CTRL	=	BSP_SIU_INT_IRQ_3,
  /*
     * Some CPM IRQ symbolic name definition
     */
  BSP_CPM_IRQ_ERROR		=	BSP_CPM_IRQ_LOWEST_OFFSET,
  BSP_CPM_IRQ_PARALLEL_IO_PC4	=	BSP_CPM_IRQ_LOWEST_OFFSET + 1,
  BSP_CPM_IRQ_PARALLEL_IO_PC5	=	BSP_CPM_IRQ_LOWEST_OFFSET + 2,
  BSP_CPM_IRQ_SMC2_OR_PIP	=	BSP_CPM_IRQ_LOWEST_OFFSET + 3,
  BSP_CPM_IRQ_SMC1		=	BSP_CPM_IRQ_LOWEST_OFFSET + 4,
  BSP_CPM_IRQ_SPI		=	BSP_CPM_IRQ_LOWEST_OFFSET + 5,
  BSP_CPM_IRQ_PARALLEL_IO_PC6	=	BSP_CPM_IRQ_LOWEST_OFFSET + 6,
  BSP_CPM_IRQ_TIMER_4		=	BSP_CPM_IRQ_LOWEST_OFFSET + 7,
  
  BSP_CPM_IRQ_PARALLEL_IO_PC7	=	BSP_CPM_IRQ_LOWEST_OFFSET + 9,
  BSP_CPM_IRQ_PARALLEL_IO_PC8	=	BSP_CPM_IRQ_LOWEST_OFFSET + 10,
  BSP_CPM_IRQ_PARALLEL_IO_PC9	=	BSP_CPM_IRQ_LOWEST_OFFSET + 11,
  BSP_CPM_IRQ_TIMER_3		=	BSP_CPM_IRQ_LOWEST_OFFSET + 12,
  
  BSP_CPM_IRQ_PARALLEL_IO_PC10	=	BSP_CPM_IRQ_LOWEST_OFFSET + 14,
  BSP_CPM_IRQ_PARALLEL_IO_PC11	=	BSP_CPM_IRQ_LOWEST_OFFSET + 15,
  BSP_CPM_I2C			=	BSP_CPM_IRQ_LOWEST_OFFSET + 16,
  BSP_CPM_RISC_TIMER_TABLE	=	BSP_CPM_IRQ_LOWEST_OFFSET + 17,
  BSP_CPM_IRQ_TIMER_2		=	BSP_CPM_IRQ_LOWEST_OFFSET + 18,
  
  BSP_CPM_IDMA2			=	BSP_CPM_IRQ_LOWEST_OFFSET + 20,
  BSP_CPM_IDMA1			=	BSP_CPM_IRQ_LOWEST_OFFSET + 21,
  BSP_CPM_SDMA_CHANNEL_BUS_ERR	=	BSP_CPM_IRQ_LOWEST_OFFSET + 22,
  BSP_CPM_IRQ_PARALLEL_IO_PC12	=	BSP_CPM_IRQ_LOWEST_OFFSET + 23,
  BSP_CPM_IRQ_PARALLEL_IO_PC13	=	BSP_CPM_IRQ_LOWEST_OFFSET + 24,
  BSP_CPM_IRQ_TIMER_1		=	BSP_CPM_IRQ_LOWEST_OFFSET + 25,
  BSP_CPM_IRQ_PARALLEL_IO_PC14	=	BSP_CPM_IRQ_LOWEST_OFFSET + 26,
  BSP_CPM_IRQ_SCC4		=	BSP_CPM_IRQ_LOWEST_OFFSET + 27,
  BSP_CPM_IRQ_SCC3		=	BSP_CPM_IRQ_LOWEST_OFFSET + 28,
  BSP_CPM_IRQ_SCC2		=	BSP_CPM_IRQ_LOWEST_OFFSET + 29,
  BSP_CPM_IRQ_SCC1		=	BSP_CPM_IRQ_LOWEST_OFFSET + 30,
  BSP_CPM_IRQ_PARALLEL_IO_PC15	=	BSP_CPM_IRQ_LOWEST_OFFSET + 31,
    /*
     * Some Processor exception handled as rtems IRQ symbolic name definition
     */
  BSP_DECREMENTER		=	BSP_PROCESSOR_IRQ_LOWEST_OFFSET
  
}rtems_irq_symbolic_name;

#define CPM_INTERRUPT		    


/*
 * Type definition for RTEMS managed interrupts
 */
typedef unsigned char  rtems_irq_prio;
struct 	__rtems_irq_connect_data__;	/* forward declaratiuon */

typedef void (*rtems_irq_hdl)		(void);
typedef void (*rtems_irq_enable)	(const struct __rtems_irq_connect_data__*);
typedef void (*rtems_irq_disable)	(const struct __rtems_irq_connect_data__*);
typedef int  (*rtems_irq_is_enabled)	(const struct __rtems_irq_connect_data__*);

typedef struct __rtems_irq_connect_data__ {
  /*
   * IRQ line
   */
  rtems_irq_symbolic_name 	name;
  /*
   * handler. See comment on handler properties below in function prototype.
   */
  rtems_irq_hdl	   		hdl;
  /*
   * function for enabling interrupts at device level (ONLY!).
   * The BSP code will automatically enable it at SIU level and CPM level.
   * RATIONALE : anyway such code has to exist in current driver code.
   * It is usually called immediately AFTER connecting the interrupt handler.
   * RTEMS may well need such a function when restoring normal interrupt
   * processing after a debug session.
   * 
   */
    rtems_irq_enable		on;	
  /*
   * function for disabling interrupts at device level (ONLY!).
   * The code will disable it at SIU and CPM level. RATIONALE : anyway
   * such code has to exist for clean shutdown. It is usually called
   * BEFORE disconnecting the interrupt. RTEMS may well need such
   * a function when disabling normal interrupt processing for
   * a debug session. May well be a NOP function.
   */
  rtems_irq_disable		off;
  /*
   * function enabling to know what interrupt may currently occur
   * if someone manipulates the i8259s interrupt mask without care...
   */
    rtems_irq_is_enabled	isOn;
}rtems_irq_connect_data;

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
   * actual value of BSP_SIU_IRQ_VECTOR_BASE...
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
}rtems_irq_global_settings;




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
int BSP_irq_disable_at_siu        (const rtems_irq_symbolic_name irqLine);
/*
 * function to enable a particular irq at 8259 level. After calling
 * this function, if the device asserts the interrupt line it will
 * be propagated further to the processor
 */
int BSP_irq_enable_at_siu		(const rtems_irq_symbolic_name irqLine);
/*
 * function to acknoledge a particular irq at 8259 level. After calling
 * this function, if a device asserts an enabled interrupt line it will
 * be propagated further to the processor. Mainly usefull for people
 * writting raw handlers as this is automagically done for rtems managed
 * handlers.
 */
int BSP_irq_ack_at_siu              	(const rtems_irq_symbolic_name irqLine);
/*
 * function to check if a particular irq is enabled at 8259 level. After calling
 */
int BSP_irq_enabled_at_siu     	(const rtems_irq_symbolic_name irqLine);
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
int BSP_install_rtems_irq_handler   	(const rtems_irq_connect_data*);
/*
 * function to get the current RTEMS irq handler for ptr->name. It enables to
 * define hanlder chain...
 */
int BSP_get_current_rtems_irq_handler	(rtems_irq_connect_data* ptr);
/*
 * function to get disconnect the RTEMS irq handler for ptr->name.
 * This function checks that the value given is the current one for safety reason.
 * The user can use the previous function to get it.
 */
int BSP_remove_rtems_irq_handler    	(const rtems_irq_connect_data*);

/*
 * ------------------------ RTEMS Global Irq Handler Mngt Routines ----------------
 */
/*
 * (Re) Initialize the RTEMS interrupt management.
 *
 * The result of calling this function will be the same as if each individual
 * handler (config->irqHdlTbl[i].hdl)  different from "config->defaultEntry.hdl"
 * has been individualy connected via
 *	BSP_install_rtems_irq_handler(&config->irqHdlTbl[i])
 * And each handler currently equal to config->defaultEntry.hdl
 * has been previously disconnected via
 * 	 BSP_remove_rtems_irq_handler (&config->irqHdlTbl[i])
 *
 * This is to say that all information given will be used and not just
 * only the space.
 *
 * CAUTION : the various table address contained in config will be used
 *	     directly by the interrupt mangement code in order to save
 *	     data size so they must stay valid after the call => they should
 *	     not be modified or declared on a stack.
 */

int BSP_rtems_irq_mngt_set(rtems_irq_global_settings* config);
/*
 * (Re) get info on current RTEMS interrupt management.
 */
int BSP_rtems_irq_mngt_get(rtems_irq_global_settings**);
  
extern void BSP_rtems_irq_mng_init(unsigned cpuId);

#ifdef __cplusplus
}
#endif

#endif

#endif
