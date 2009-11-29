/*
 * Cirrus EP7312 Intererrupt handler
 *
 * Copyright (c) 2002 by Jay Monkman <jtm@smoothsmoothie.com>
 *
 * Copyright (c) 2002 by Charlie Steader <charlies@poliac.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.com/license/LICENSE.
 *
 *
 *  $Id$
*/

#ifndef __IRQ_H__
#define __IRQ_H__

#ifdef __cplusplus
extern "C" {
#endif

  /* define that can be useful (the values are just examples) */
#define VECTOR_TABLE	 0x40

#ifndef __asm__

/*
 * Include some preprocessor value also used by assember code
 */

#include <rtems.h>

extern void default_int_handler();
/*-------------------------------------------------------------------------+
| Constants
+--------------------------------------------------------------------------*/

  /* enum of the possible interrupt sources */
typedef unsigned int rtems_irq_number;
  /* int interrupt status/mask register 1 */
#define BSP_EXTFIQ     0
#define BSP_BLINT      1
#define BSP_WEINT      2
#define BSP_MCINT      3
#define BSP_CSINT      4
#define BSP_EINT1      5
#define BSP_EINT2      6
#define BSP_EINT3      7
#define BSP_TC1OI      8
#define BSP_TC2OI      9
#define BSP_RTCMI     10
#define BSP_TINT      11
#define BSP_UTXINT1   12
#define BSP_URXINT1   13
#define BSP_UMSINT    14
#define BSP_SSEOTI    15
 /* int interrupt status/mask register 2 */
#define BSP_KBDINT    16
#define BSP_SS2RX     17
#define BSP_SS2TX     18
#define BSP_UTXINT2   19
#define BSP_URXINT2   20
 /* int interrupt status/mask register 3 */
#define BSP_DAIINT    21
#define BSP_MAX_INT   22

/*
 * Type definition for RTEMS managed interrupts
 */
typedef unsigned char  rtems_irq_level;
typedef unsigned char  rtems_irq_trigger;

struct 	__rtems_irq_connect_data__;	/* forward declaratiuon */

typedef void (*rtems_irq_hdl)		(void);
typedef void (*rtems_irq_enable) (const struct __rtems_irq_connect_data__*);
typedef void (*rtems_irq_disable) (const struct __rtems_irq_connect_data__*);
typedef int  (*rtems_irq_is_enabled)(const struct __rtems_irq_connect_data__*);

typedef struct __rtems_irq_connect_data__ {
  /*
   * IRQ line
   */
  rtems_irq_number   		name;
  /*
   * handler. See comment on handler properties below in function prototype.
   */
  rtems_irq_hdl	   		hdl;
  /*
   * function for enabling interrupts at device level (ONLY!).
   * The BSP code will automatically enable it at i8259s level.
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
   * priority level at the vplus level
   */
  rtems_irq_level		irqLevel;
  /*
   * Trigger way : Rising or falling edge or High or low level
   */
  rtems_irq_trigger		irqTrigger;
} rtems_irq_connect_data;

/*-------------------------------------------------------------------------+
| Function Prototypes.
+--------------------------------------------------------------------------*/
/*
 * ------------------- RTEMS Single Irq Handler Mngt Routines ----------------
 */

/*
 * function to initialize the interrupt for a specific BSP
 */
void BSP_rtems_irq_mngt_init();

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

#endif /* __asm__ */

#ifdef __cplusplus
}
#endif

#endif /* __IRQ_H__ */
