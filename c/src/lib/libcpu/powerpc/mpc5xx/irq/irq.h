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
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef LIBCPU_POWERPC_MBX8XX_IRQ_IRQ_H
#define LIBCPU_POWERPC_MBX8XX_IRQ_IRQ_H


#define CPU_ASM_IRQ_VECTOR_BASE 0x0

#ifndef ASM

extern volatile unsigned int ppc_cached_irq_mask;

/*
 * Symblolic IRQ names and related definitions.
 */

typedef enum {
  /* 
   * Base vector for our USIU IRQ handlers. 
   */
  CPU_USIU_VECTOR_BASE		= CPU_ASM_IRQ_VECTOR_BASE,
  /*
   * USIU IRQ handler related definitions
   */
  CPU_USIU_IRQ_COUNT    	= 16, /* 16 reserved but in the future... */
  CPU_USIU_IRQ_MIN_OFFSET	= 0,
  CPU_USIU_IRQ_MAX_OFFSET	= CPU_USIU_IRQ_MIN_OFFSET + CPU_USIU_IRQ_COUNT - 1,
  /*
   * UIMB IRQ handlers related definitions
   */
  CPU_UIMB_IRQ_COUNT		= 32 - 8, /* first 8 overlap USIU */
  CPU_UIMB_IRQ_MIN_OFFSET	= CPU_USIU_IRQ_COUNT + CPU_USIU_VECTOR_BASE,
  CPU_UIMB_IRQ_MAX_OFFSET	= CPU_UIMB_IRQ_MIN_OFFSET + CPU_UIMB_IRQ_COUNT - 1,
  /*
   * PowerPc exceptions handled as interrupt where a rtems managed interrupt
   * handler might be connected
   */
  CPU_PROC_IRQ_COUNT		= 1,
  CPU_PROC_IRQ_MIN_OFFSET	= CPU_UIMB_IRQ_MAX_OFFSET + 1,
  CPU_PROC_IRQ_MAX_OFFSET	= CPU_PROC_IRQ_MIN_OFFSET + CPU_PROC_IRQ_COUNT - 1,
  /*
   * Summary
   */
  CPU_IRQ_COUNT			= CPU_PROC_IRQ_MAX_OFFSET + 1,
  CPU_MIN_OFFSET		= CPU_USIU_IRQ_MIN_OFFSET,
  CPU_MAX_OFFSET		= CPU_PROC_IRQ_MAX_OFFSET,
  /*
   * USIU IRQ symbolic name definitions.
   */	       
  CPU_USIU_EXT_IRQ_0      	= CPU_USIU_IRQ_MIN_OFFSET,
  CPU_USIU_INT_IRQ_0,

  CPU_USIU_EXT_IRQ_1,
  CPU_USIU_INT_IRQ_1,

  CPU_USIU_EXT_IRQ_2,
  CPU_USIU_INT_IRQ_2,
  
  CPU_USIU_EXT_IRQ_3,
  CPU_USIU_INT_IRQ_3,
  
  CPU_USIU_EXT_IRQ_4,
  CPU_USIU_INT_IRQ_4,

  CPU_USIU_EXT_IRQ_5,
  CPU_USIU_INT_IRQ_5,
  
  CPU_USIU_EXT_IRQ_6,
  CPU_USIU_INT_IRQ_6,
  
  CPU_USIU_EXT_IRQ_7,
  CPU_USIU_INT_IRQ_7,

  /*
   * Symbolic names for UISU interrupt sources.
   */
  CPU_PERIODIC_TIMER		= CPU_USIU_INT_IRQ_6,
  CPU_UIMB_INTERRUPT		= CPU_USIU_INT_IRQ_7,
              
  /*
   * UIMB IRQ symbolic name definitions.  The first 8 sources are aliases to
   * the USIU interrupts of the same number, because they are detected in
   * the USIU pending register rather than the UIMB pending register.
   */
  CPU_UIMB_IRQ_0 		= CPU_USIU_INT_IRQ_0,
  CPU_UIMB_IRQ_1 		= CPU_USIU_INT_IRQ_1,
  CPU_UIMB_IRQ_2 		= CPU_USIU_INT_IRQ_2,
  CPU_UIMB_IRQ_3 		= CPU_USIU_INT_IRQ_3,
  CPU_UIMB_IRQ_4 		= CPU_USIU_INT_IRQ_4,
  CPU_UIMB_IRQ_5 		= CPU_USIU_INT_IRQ_5,
  CPU_UIMB_IRQ_6 		= CPU_USIU_INT_IRQ_6,
  CPU_UIMB_IRQ_7 		= CPU_USIU_INT_IRQ_7,

  CPU_UIMB_IRQ_8         	= CPU_UIMB_IRQ_MIN_OFFSET,
  CPU_UIMB_IRQ_9,
  CPU_UIMB_IRQ_10,
  CPU_UIMB_IRQ_11,
  CPU_UIMB_IRQ_12,
  CPU_UIMB_IRQ_13,
  CPU_UIMB_IRQ_14,
  CPU_UIMB_IRQ_15,
  CPU_UIMB_IRQ_16,
  CPU_UIMB_IRQ_17,
  CPU_UIMB_IRQ_18,
  CPU_UIMB_IRQ_19,
  CPU_UIMB_IRQ_20,
  CPU_UIMB_IRQ_21,
  CPU_UIMB_IRQ_22,
  CPU_UIMB_IRQ_23,
  CPU_UIMB_IRQ_24,
  CPU_UIMB_IRQ_25,
  CPU_UIMB_IRQ_26,
  CPU_UIMB_IRQ_27,
  CPU_UIMB_IRQ_28,
  CPU_UIMB_IRQ_29,
  CPU_UIMB_IRQ_30,
  CPU_UIMB_IRQ_31,
  
  /*
   * Symbolic names for UIMB interrupt sources.
   */
  CPU_IRQ_SCI			= CPU_UIMB_IRQ_5,

  /*
   * Processor exceptions handled as rtems IRQ symbolic name definitions.
   */
  CPU_DECREMENTER		= CPU_PROC_IRQ_MIN_OFFSET
     
}rtems_irq_symbolic_name;

/*
 * Convert an rtems_irq_symbolic_name constant to an interrupt level
 * suitable for programming into an I/O device's interrupt level field.
 */
int CPU_irq_level_from_symbolic_name(const rtems_irq_symbolic_name name);

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
   * Handler. See comment on handler properties below in function prototype.
   */
  rtems_irq_hdl	   		hdl;
  /*
   * Function for enabling interrupts at device level (ONLY!).
   * The CPU code will automatically enable it at USIU level and UIMB level.
   * RATIONALE : anyway such code has to exist in current driver code.
   * It is usually called immediately AFTER connecting the interrupt handler.
   * RTEMS may well need such a function when restoring normal interrupt
   * processing after a debug session.
   * 
   */
    rtems_irq_enable		on;	
  /*
   * Function for disabling interrupts at device level (ONLY!).
   * The code will disable it at USIU and UIMB level. RATIONALE : anyway
   * such code has to exist for clean shutdown. It is usually called
   * BEFORE disconnecting the interrupt. RTEMS may well need such
   * a function when disabling normal interrupt processing for
   * a debug session. May well be a NOP function.
   */
  rtems_irq_disable		off;
  /*
   * Function enabling to know what interrupt may currently occur
   * if someone manipulates the USIU and UIMB interrupt mask without care...
   */
  rtems_irq_is_enabled		isOn;
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
   * actual value of CPU_USIU_IRQ_VECTOR_BASE...
   */
  rtems_irq_symbolic_name	irqBase;
  /*
   * software priorities associated with interrupts.
   * if irqPrio  [i]  >  intrPrio  [j]  it  means  that  
   * interrupt handler hdl connected for interrupt name i
   * will  not be interrupted by the handler connected for interrupt j
   * The interrupt source  will be physically masked at USIU and UIMB level.
   */
  rtems_irq_prio*		irqPrioTbl;
}rtems_irq_global_settings;


/*-------------------------------------------------------------------------+
| Function Prototypes.
+--------------------------------------------------------------------------*/

#if 0
/*
 * -------------------- MPC5xx USIU Management Routines -----------------
 */
/*
 * Function to disable a particular irq at USIU level.  After calling
 * this function, even if the device asserts the interrupt line it will
 * not be propagated further to the processor
 */
int CPU_irq_disable_at_usiu	(const rtems_irq_symbolic_name irqLine);
/*
 * Function to enable a particular irq at USIU level.  After calling
 * this function, if the device asserts the interrupt line it will
 * be propagated further to the processor
 */
int CPU_irq_enable_at_usiu	(const rtems_irq_symbolic_name irqLine);
/*
 * Function to acknowledge a particular irq at USIU level.  After calling
 * this function, if a device asserts an enabled interrupt line it will
 * be propagated further to the processor. Mainly useful for people
 * writing raw handlers as this is automagically done for rtems managed
 * handlers.
 */
int CPU_irq_ack_at_siu		(const rtems_irq_symbolic_name irqLine);
/*
 * function to check if a particular irq is enabled at USIU level.
 */
int CPU_irq_enabled_at_siu	(const rtems_irq_symbolic_name irqLine);

#endif

/*
 * ------------ RTEMS Single Irq Handler Management Routines ----------------
 */
/*
 * Function to connect a particular irq handler. This handler will NOT be
 * called directly as the result of the corresponding interrupt. Instead, a
 * RTEMS irq prologue will be called that will :
 *
 *	1) save the C scratch registers,
 *	2) switch to a interrupt stack if the interrupt is not nested,
 *	4) modify them to disable the current interrupt at  USIU level (and
 *	   maybe others depending on software priorities)
 *	5) aknowledge the USIU',
 *	6) demask the processor,
 *	7) call the application handler
 *
 * As a result the hdl function provided
 *
 *	a) can perfectly be written is C,
 * 	b) may also well directly call the part of the RTEMS API that can be
 *	   used from interrupt level,
 *	c) It only responsible for handling the jobs that need to be done at
 *	   the device level including (aknowledging/re-enabling the
 *	   interrupt at device, level, getting the data,...)
 *
 *	When returning from the function, the following will be performed by
 *	the RTEMS irq epilogue :
 *
 *	1) masks the interrupts again,
 *	2) restore the original USIU interrupt masks
 *	3) switch back on the orinal stack if needed,
 *	4) perform rescheduling when necessary,
 *	5) restore the C scratch registers...
 *	6) restore initial execution flow
 * 
 */
int CPU_install_rtems_irq_handler   	(const rtems_irq_connect_data*);
/*
 * Function to connect an RTEMS irq handler for ptr->name.
 */
int CPU_get_current_rtems_irq_handler	(rtems_irq_connect_data* ptr);
/*
 * Function to get the RTEMS irq handler for ptr->name.
 */
int CPU_remove_rtems_irq_handler    	(const rtems_irq_connect_data*);
/*
 * Function to disconnect the RTEMS irq handler for ptr->name.  This
 * function checks that the value given is the current one for safety
 * reason.  The user can use the previous function to get it.
 */

/*
 * ------------ RTEMS Global Irq Handler Management Routines ----------------
 */
/*
 * (Re) Initialize the RTEMS interrupt management.
 *
 * The result of calling this function will be the same as if each
 * individual handler (config->irqHdlTbl[i].hdl) different from
 * "config->defaultEntry.hdl" has been individualy connected via
 *
 *	CPU_install_rtems_irq_handler(&config->irqHdlTbl[i])
 *
 * and each handler currently equal to config->defaultEntry.hdl
 * has been previously disconnected via
 *
 * 	 CPU_remove_rtems_irq_handler (&config->irqHdlTbl[i])
 *
 * This is to say that all information given will be used and not just
 * only the space.
 *
 * CAUTION : the various table address contained in config will be used
 *	     directly by the interrupt mangement code in order to save
 *	     data size so they must stay valid after the call => they should
 *	     not be modified or declared on a stack.
 */

int CPU_rtems_irq_mngt_set(rtems_irq_global_settings* config);
/*
 * (Re) get info on current RTEMS interrupt management.
 */
int CPU_rtems_irq_mngt_get(rtems_irq_global_settings**);
  
extern void CPU_rtems_irq_mng_init(unsigned cpuId);

#endif

#endif
