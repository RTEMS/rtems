/* irq.h
 *
 *  This include file describe the data structure and the functions implemented
 *  by RTEMS to write interrupt handlers.
 *
 *  Copyright (C) 1999 valette@crf.canon.fr
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

#ifndef LIBBSP_POWERPC_MCP750_IRQ_IRQ_H
#define LIBBSP_POWERPC_MCP750_IRQ_IRQ_H

/*
 * 8259 edge/level control definitions at VIA
 */
#define ISA8259_M_ELCR 		0x4d0
#define ISA8259_S_ELCR 		0x4d1

#define ELCRS_INT15_LVL         0x80
#define ELCRS_INT14_LVL         0x40
#define ELCRS_INT13_LVL         0x20
#define ELCRS_INT12_LVL         0x10
#define ELCRS_INT11_LVL         0x08
#define ELCRS_INT10_LVL         0x04
#define ELCRS_INT9_LVL          0x02
#define ELCRS_INT8_LVL          0x01
#define ELCRM_INT7_LVL          0x80
#define ELCRM_INT6_LVL          0x40
#define ELCRM_INT5_LVL          0x20
#define ELCRM_INT4_LVL          0x10
#define ELCRM_INT3_LVL          0x8
#define ELCRM_INT2_LVL          0x4
#define ELCRM_INT1_LVL          0x2
#define ELCRM_INT0_LVL          0x1

#define BSP_ASM_IRQ_VECTOR_BASE 0x0
    /* PIC's command and mask registers */
#define PIC_MASTER_COMMAND_IO_PORT		0x20	/* Master PIC command register */
#define PIC_SLAVE_COMMAND_IO_PORT		0xa0	/* Slave PIC command register */
#define PIC_MASTER_IMR_IO_PORT			0x21	/* Master PIC Interrupt Mask Register */
#define PIC_SLAVE_IMR_IO_PORT		 	0xa1	/* Slave PIC Interrupt Mask Register */

    /* Command for specific EOI (End Of Interrupt): Interrupt acknowledge */
#define PIC_EOSI	0x60	/* End of Specific Interrupt (EOSI) */
#define	SLAVE_PIC_EOSI  0x62	/* End of Specific Interrupt (EOSI) for cascade */
#define PIC_EOI		0x20	/* Generic End of Interrupt (EOI) */

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Symbolic IRQ names and related definitions.
 */

typedef enum {
  /* Base vector for our ISA IRQ handlers. */
  BSP_ISA_IRQ_VECTOR_BASE	= BSP_ASM_IRQ_VECTOR_BASE,
  /*
   * ISA IRQ handler related definitions
   */
  BSP_ISA_IRQ_NUMBER    	= 16,
  BSP_ISA_IRQ_LOWEST_OFFSET	= 0,
  BSP_ISA_IRQ_MAX_OFFSET	= BSP_ISA_IRQ_LOWEST_OFFSET + BSP_ISA_IRQ_NUMBER - 1,
  /*
   * PCI IRQ handlers related definitions
   * CAUTION : BSP_PCI_IRQ_LOWEST_OFFSET should be equal to OPENPIC_VEC_SOURCE
   */
  BSP_PCI_IRQ_NUMBER		= 16,
  BSP_PCI_IRQ_LOWEST_OFFSET	= BSP_ISA_IRQ_NUMBER,
  BSP_PCI_IRQ_MAX_OFFSET	= BSP_PCI_IRQ_LOWEST_OFFSET + BSP_PCI_IRQ_NUMBER - 1,
  /*
   * PowerPC exceptions handled as interrupt where an RTEMS managed interrupt
   * handler might be connected
   */
  BSP_PROCESSOR_IRQ_NUMBER	= 1,
  BSP_PROCESSOR_IRQ_LOWEST_OFFSET = BSP_PCI_IRQ_MAX_OFFSET + 1,
  BSP_PROCESSOR_IRQ_MAX_OFFSET	= BSP_PROCESSOR_IRQ_LOWEST_OFFSET + BSP_PROCESSOR_IRQ_NUMBER - 1,
  /* Misc vectors for OPENPIC irqs (IPI, timers)
   */
  BSP_MISC_IRQ_NUMBER		= 8,
  BSP_MISC_IRQ_LOWEST_OFFSET	= BSP_PROCESSOR_IRQ_MAX_OFFSET + 1,
  BSP_MISC_IRQ_MAX_OFFSET	= BSP_MISC_IRQ_LOWEST_OFFSET + BSP_MISC_IRQ_NUMBER - 1,
  /*
   * Summary
   */
  BSP_IRQ_NUMBER		= BSP_MISC_IRQ_MAX_OFFSET + 1,
  BSP_LOWEST_OFFSET		= BSP_ISA_IRQ_LOWEST_OFFSET,
  BSP_MAX_OFFSET		= BSP_MISC_IRQ_MAX_OFFSET,
    /*
     * Some ISA IRQ symbolic name definition
     */	
  BSP_ISA_PERIODIC_TIMER      	=  0,
  BSP_ISA_KEYBOARD          	=  1,
  BSP_ISA_UART_COM2_IRQ		=  3,
  BSP_ISA_UART_COM1_IRQ		=  4,
  BSP_ISA_RT_TIMER1	      	=  8,
  BSP_ISA_RT_TIMER3		= 10,
    /*
     * Some PCI IRQ symbolic name definition
     */
  BSP_PCI_IRQ0			= BSP_PCI_IRQ_LOWEST_OFFSET,
  BSP_PCI_ISA_BRIDGE_IRQ	= BSP_PCI_IRQ0,

#if defined(mvme2100)
  BSP_DEC21143_IRQ                = BSP_PCI_IRQ_LOWEST_OFFSET + 1,
  BSP_PMC_PCMIP_TYPE1_SLOT0_IRQ   = BSP_PCI_IRQ_LOWEST_OFFSET + 2,
  BSP_PCMIP_TYPE1_SLOT1_IRQ       = BSP_PCI_IRQ_LOWEST_OFFSET + 3,
  BSP_PCMIP_TYPE2_SLOT0_IRQ       = BSP_PCI_IRQ_LOWEST_OFFSET + 4,
  BSP_PCMIP_TYPE2_SLOT1_IRQ       = BSP_PCI_IRQ_LOWEST_OFFSET + 5,
  BSP_PCI_INTA_UNIVERSE_LINT0_IRQ = BSP_PCI_IRQ_LOWEST_OFFSET + 7,
  BSP_PCI_INTB_UNIVERSE_LINT1_IRQ = BSP_PCI_IRQ_LOWEST_OFFSET + 8,
  BSP_PCI_INTC_UNIVERSE_LINT2_IRQ = BSP_PCI_IRQ_LOWEST_OFFSET + 9,
  BSP_PCI_INTD_UNIVERSE_LINT3_IRQ = BSP_PCI_IRQ_LOWEST_OFFSET + 10,
  BSP_UART_COM1_IRQ               = BSP_PCI_IRQ_LOWEST_OFFSET + 13,
  BSP_FRONT_PANEL_ABORT_IRQ       = BSP_PCI_IRQ_LOWEST_OFFSET + 14,
  BSP_RTC_IRQ                     = BSP_PCI_IRQ_LOWEST_OFFSET + 15,
#endif

    /*
     * Some Processor exception handled as RTEMS IRQ symbolic name definition
     */
  BSP_DECREMENTER		= BSP_PROCESSOR_IRQ_LOWEST_OFFSET

} rtems_irq_symbolic_name;

/*
 * Type definition for RTEMS managed interrupts
 */
typedef unsigned char  rtems_irq_prio;
typedef unsigned short rtems_i8259_masks;

extern  volatile rtems_i8259_masks i8259s_cache;

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
       * The BSP code will automatically enable it at i8259s level and openpic level.
       * RATIONALE : anyway such code has to exist in current driver code.
       * It is usually called immediately AFTER connecting the interrupt handler.
       * RTEMS may well need such a function when restoring normal interrupt
       * processing after a debug session.
       */
      rtems_irq_enable		on;	
      /*
       * function for disabling interrupts at device level (ONLY!).
       * The code will disable it at i8259s level. RATIONALE : anyway
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
      /*
       *  Set to -1 for vectors forced to have only 1 handler
       */
      void *next_handler;

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
   * actual value of BSP_ISA_IRQ_VECTOR_BASE...
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
 * ------------------------ Intel 8259 (or emulation) Mngt Routines -------
 */

/*
 * function to disable a particular irq at 8259 level. After calling
 * this function, even if the device asserts the interrupt line it will
 * not be propagated further to the processor
 */
int BSP_irq_disable_at_i8259s        (const rtems_irq_symbolic_name irqLine);
/*
 * function to enable a particular irq at 8259 level. After calling
 * this function, if the device asserts the interrupt line it will
 * be propagated further to the processor
 */
int BSP_irq_enable_at_i8259s		(const rtems_irq_symbolic_name irqLine);
/*
 * function to acknowledge a particular irq at 8259 level. After calling
 * this function, if a device asserts an enabled interrupt line it will
 * be propagated further to the processor. Mainly useful for people
 * writing raw handlers as this is automagically done for RTEMS managed
 * handlers.
 */
int BSP_irq_ack_at_i8259s           	(const rtems_irq_symbolic_name irqLine);
/*
 * function to check if a particular irq is enabled at 8259 level. After calling
 */
int BSP_irq_enabled_at_i8259s        	(const rtems_irq_symbolic_name irqLine);
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
 *	3) store the current i8259s' interrupt masks
 *	4) modify them to disable the current interrupt at 8259 level (and may
 *	be others depending on software priorities)
 *	5) acknowledge the i8259s',
 *	6) demask the processor,
 *	7) call the application handler
 *
 * As a result the hdl function provided
 *
 *	a) can perfectly be written is C,
 * 	b) may also well directly call the part of the RTEMS API that can be
 *         used from interrupt level,
 *	c) It is only responsible for handling the jobs that need to be done at
 *	   the device level including (acknowledging/re-enabling the interrupt
 *         at the device level, getting the data,...)
 *
 *	When returning from the function, the following will be performed by
 *	the RTEMS irq epilogue :
 *
 *	1) masks the interrupts again,
 *	2) restore the original i8259s' interrupt masks
 *	3) switch back on the orinal stack if needed,
 *	4) perform rescheduling when necessary,
 *	5) restore the C scratch registers...
 *	6) restore initial execution flow
 */
int BSP_install_rtems_irq_handler   	(const rtems_irq_connect_data*);
int BSP_install_rtems_shared_irq_handler  (const rtems_irq_connect_data*);

#define BSP_SHARED_HANDLER_SUPPORT      1

/*
 * function to get the current RTEMS irq handler for ptr->name. It enables to
 * define hanlder chain...
 */
int BSP_get_current_rtems_irq_handler	(rtems_irq_connect_data* ptr);
/*
 * function to get disconnect the RTEMS irq handler for ptr->name.
 * This function checks that the value given is the current one for safety
 * reasons.
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
extern void BSP_i8259s_init(void);

#ifdef __cplusplus
}
#endif

#endif

#endif
