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
 * Copyright 2004, Brookhaven National Laboratory and
 *                 Shuchen Kate Feng <feng1@bnl.gov> 
 *
 *    - modified shared/irq/irq.h for Mvme5500 (no ISA devices/PIC)
 *    - Discovery GT64260 interrupt controller instead of 8259.
 *    - Added support for software IRQ priority levels. 
 *
 *  $Id$
 */

#ifndef LIBBSP_POWERPC_MVME5500_IRQ_IRQ_H
#define LIBBSP_POWERPC_MVME5500_IRQ_IRQ_H


#define BSP_ASM_IRQ_VECTOR_BASE 0x0

#ifndef ASM

#define DynamicIrqTbl 1

/*
 * Symbolic IRQ names and related definitions.
 */

/* leave the ISA symbols in there, so we can reuse shared/irq.c
 * Also, we start numbering PCI irqs at 16 because the OPENPIC
 * driver relies on this when mapping irq number <-> vectors
 * (OPENPIC_VEC_SOURCE in openpic.h)
 */

typedef enum {
  /* See section 25.2 , Table 734 of GT64260 controller 
   * Main Interrupt Cause Low register
   */
  BSP_MICL_IRQ_NUMBER           = 32,
  BSP_MICL_IRQ_LOWEST_OFFSET    =  0,
  BSP_MICL_IRQ_MAX_OFFSET       =  BSP_MICL_IRQ_LOWEST_OFFSET + BSP_MICL_IRQ_NUMBER -1,
  /* 
   * Main Interrupt Cause High register
   */
  BSP_MICH_IRQ_NUMBER           = 32,
  BSP_MICH_IRQ_LOWEST_OFFSET    = BSP_MICL_IRQ_MAX_OFFSET+1,
  BSP_MICH_IRQ_MAX_OFFSET       = BSP_MICH_IRQ_LOWEST_OFFSET + BSP_MICH_IRQ_NUMBER -1,
  /* External GPP Interrupt assignements
   */
  BSP_GPP_IRQ_NUMBER		=  32,
  BSP_GPP_IRQ_LOWEST_OFFSET	=  BSP_MICH_IRQ_MAX_OFFSET+1, 
  BSP_GPP_IRQ_MAX_OFFSET	=  BSP_GPP_IRQ_LOWEST_OFFSET + BSP_GPP_IRQ_NUMBER - 1,

  /*
   * PowerPc exceptions handled as interrupt where a rtems managed interrupt
   * handler might be connected
   */
  BSP_PROCESSOR_IRQ_NUMBER	=   1,
  BSP_PROCESSOR_IRQ_LOWEST_OFFSET = BSP_GPP_IRQ_MAX_OFFSET + 1,
  BSP_PROCESSOR_IRQ_MAX_OFFSET	=   BSP_PROCESSOR_IRQ_LOWEST_OFFSET + BSP_PROCESSOR_IRQ_NUMBER - 1,

  /* allow a couple of vectors for VME and counter/timer irq sources etc.
   * This is probably not needed any more. 
   */
  BSP_MISC_IRQ_NUMBER		=   30,
  BSP_MISC_IRQ_LOWEST_OFFSET	=   BSP_PROCESSOR_IRQ_MAX_OFFSET + 1,
  BSP_MISC_IRQ_MAX_OFFSET	=   BSP_MISC_IRQ_LOWEST_OFFSET + BSP_MISC_IRQ_NUMBER - 1,

#if 0
  /*
   * ISA IRQ handler related definitions
   */
  /* MVME5500 ISA local resources exist only if an IPMC 712/761 module
   * is mounted.
   */
  BSP_ISA_IRQ_NUMBER    	= 0, 
  BSP_ISA_IRQ_LOWEST_OFFSET	= BSP_MISC_IRQ_MAX_OFFSET+1,
  BSP_ISA_IRQ_MAX_OFFSET	= BSP_ISA_IRQ_LOWEST_OFFSET + BSP_ISA_IRQ_NUMBER - 1,

#endif

  /*
   * Summary
   */
  BSP_IRQ_NUMBER		= BSP_MISC_IRQ_MAX_OFFSET + 1,
  BSP_MAIN_IRQ_NUMBER           = 64,
  BSP_LOWEST_OFFSET		= BSP_MICL_IRQ_LOWEST_OFFSET,
  BSP_MAX_OFFSET		= BSP_MISC_IRQ_MAX_OFFSET,

  /* Main CPU interrupt cause (Low) */
  BSP_MAIN_TIMER0_1_IRQ         = BSP_MICL_IRQ_LOWEST_OFFSET+8,
  BSP_MAIN_PCI0_7_0             = BSP_MICL_IRQ_LOWEST_OFFSET+12,
  BSP_MAIN_PCI0_15_8            = BSP_MICL_IRQ_LOWEST_OFFSET+13,
  BSP_MAIN_PCI0_23_16           = BSP_MICL_IRQ_LOWEST_OFFSET+14,
  BSP_MAIN_PCI0_31_24           = BSP_MICL_IRQ_LOWEST_OFFSET+15,
  BSP_MAIN_PCI1_7_0             = BSP_MICL_IRQ_LOWEST_OFFSET+16,
  BSP_MAIN_PCI1_15_8            = BSP_MICL_IRQ_LOWEST_OFFSET+18,
  BSP_MAIN_PCI1_23_16           = BSP_MICL_IRQ_LOWEST_OFFSET+19,
  BSP_MAIN_PCI1_31_24           = BSP_MICL_IRQ_LOWEST_OFFSET+20,


  /* Main CPU interrupt cause (High) */
  BSP_MAIN_ETH0_IRQ             = BSP_MICH_IRQ_LOWEST_OFFSET,
  BSP_MAIN_ETH1_IRQ             = BSP_MICH_IRQ_LOWEST_OFFSET+1,
  BSP_MAIN_ETH2_IRQ             = BSP_MICH_IRQ_LOWEST_OFFSET+2,
  BSP_MAIN_GPP7_0_IRQ           = BSP_MICH_IRQ_LOWEST_OFFSET+24,
  BSP_MAIN_GPP15_8_IRQ          = BSP_MICH_IRQ_LOWEST_OFFSET+25,
  BSP_MAIN_GPP23_16_IRQ         = BSP_MICH_IRQ_LOWEST_OFFSET+26,
  BSP_MAIN_GPP31_24_IRQ         = BSP_MICH_IRQ_LOWEST_OFFSET+27,

  /* on the MVME5500, these are the GT64260B external GPP0 interrupt */
  BSP_ISA_UART_COM2_IRQ		= BSP_GPP_IRQ_LOWEST_OFFSET,
  BSP_ISA_UART_COM1_IRQ		= BSP_GPP_IRQ_LOWEST_OFFSET,
  BSP_GPP8_IRQ_OFFSET           = BSP_GPP_IRQ_LOWEST_OFFSET+8,
  BSP_GPP16_IRQ_OFFSET          = BSP_GPP_IRQ_LOWEST_OFFSET+16,
  BSP_GPP24_IRQ_OFFSET          = BSP_GPP_IRQ_LOWEST_OFFSET+24,
  BSP_GPP_VME_VLINT0            = BSP_GPP_IRQ_LOWEST_OFFSET+12,
  BSP_GPP_VME_VLINT1            = BSP_GPP_IRQ_LOWEST_OFFSET+13,
  BSP_GPP_VME_VLINT2            = BSP_GPP_IRQ_LOWEST_OFFSET+14,
  BSP_GPP_VME_VLINT3            = BSP_GPP_IRQ_LOWEST_OFFSET+15,
  BSP_GPP_PMC2_INTA             = BSP_GPP_IRQ_LOWEST_OFFSET+16,
  BSP_GPP_82544_IRQ             = BSP_GPP_IRQ_LOWEST_OFFSET+20,
  BSP_GPP_WDT_NMI_IRQ           = BSP_GPP_IRQ_LOWEST_OFFSET+24,
  BSP_GPP_WDT_EXP_IRQ           = BSP_GPP_IRQ_LOWEST_OFFSET+25,

 /*
   * Some Processor execption handled as rtems IRQ symbolic name definition
   */
  BSP_DECREMENTER		=	BSP_PROCESSOR_IRQ_LOWEST_OFFSET
}rtems_irq_symbolic_name;

/*
 * Type definition for RTEMS managed interrupts
 */
typedef unsigned char  rtems_irq_prio;

typedef unsigned int rtems_GTirq_masks;

extern rtems_GTirq_masks GT_GPPirq_cache;
extern rtems_GTirq_masks GT_MAINirqLO_cache, GT_MAINirqHI_cache;

struct 	__rtems_irq_connect_data__;	/* forward declaratiuon */

typedef void (*rtems_irq_hdl)		(void);
typedef void (*rtems_irq_ack)		(void);
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
      rtems_irq_hdl	   	hdl;
      /*
       * function for enabling interrupts at device level (ONLY!).
       * The BSP code will automatically enable it at i8259s level and openpic level.
       * RATIONALE : anyway such code has to exist in current driver code.
       * It is usually called immediately AFTER connecting the interrupt handler.
       * RTEMS may well need such a function when restoring normal interrupt
       * processing after a debug session.
       * 
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
 *	5) aknowledge the i8259s',
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
 *	2) restore the original i8259s' interrupt masks
 *	3) switch back on the orinal stack if needed,
 *	4) perform rescheduling when necessary,
 *	5) restore the C scratch registers...
 *	6) restore initial execution flow
 * 
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

void BSP_enable_main_irq(unsigned irqNum);
void BSP_disable_main_irq(unsigned irqNum);
void BSP_enable_gpp_irq(unsigned irqNum);
void BSP_disable_gpp_irq(unsigned irqNum); 
 
extern void BSP_rtems_irq_mng_init(unsigned cpuId);
extern int gpp_int_error;
#if  DynamicIrqTbl
extern int MainIrqTblPtr;
extern unsigned long long MainIrqInTbl;
extern unsigned char GPPinMainIrqTbl[4];
#endif
extern unsigned int mainIrqTbl[64];
extern unsigned int GPP7_0IrqTbl[8];
extern unsigned int GPP15_8IrqTbl[8];
extern unsigned int GPP23_16IrqTbl[8];
extern unsigned int GPP31_24IrqTbl[8];

#endif

#endif
