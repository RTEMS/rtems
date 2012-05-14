/**
 * @file rtems/irq.h
 *
 * This include file describe the data structure and the functions implemented
 * by rtems to write interrupt handlers.
 */

/*
 *  CopyRight (C) 1998 valette@crf.canon.fr
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

#ifndef _RTEMS_IRQ_H
#define _RTEMS_IRQ_H

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Type definition for RTEMS managed interrupts
 */
typedef unsigned char  rtems_irq_prio;
struct  __rtems_irq_connect_data__;     /* forward declaratiuon */

typedef unsigned int rtems_irq_number;
typedef void *rtems_irq_hdl_param;
typedef void (*rtems_irq_hdl)           (rtems_irq_hdl_param);
typedef void (*rtems_irq_enable)        (const struct __rtems_irq_connect_data__*);
typedef void (*rtems_irq_disable)       (const struct __rtems_irq_connect_data__*);
typedef int  (*rtems_irq_is_enabled)    (const struct __rtems_irq_connect_data__*);

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
  unsigned int                  irqNb;
  /*
   * Default handler used when disconnecting interrupts.
   */
  rtems_irq_connect_data        defaultEntry;
  /*
   * Table containing initials/current value.
   */
  rtems_irq_connect_data*       irqHdlTbl;
  /*
   * actual value of BSP_ISA_IRQ_VECTOR_BASE...
   */
  rtems_irq_number              irqBase;
  /*
   * software priorities associated with interrupts.
   * if (*irqPrio  [i]  >  intrPrio  [j]  it  means  that
   * interrupt handler hdl connected for interrupt name i
   * will  not be interrupted by the handler connected for interrupt j
   * The interrupt source  will be physically masked at i8259 level.
   */
    rtems_irq_prio*             irqPrioTbl;
} rtems_irq_global_settings;

/*-------------------------------------------------------------------------+
| Function Prototypes.
+--------------------------------------------------------------------------*/
/*
 * -------------------- RTEMS Single Irq Handler Mngt Routines ----------------
 */
/*
 * function to connect a particular irq handler. This hanlder will NOT be called
 * directly as the result of the corresponding interrupt. Instead, a RTEMS
 * irq prologue will be called that will :
 *
 *      1) save the C scratch registers,
 *      2) switch to a interrupt stack if the interrupt is not nested,
 *      3) store the current i8259s' interrupt masks
 *      4) modify them to disable the current interrupt at 8259 level (and may
 *      be others depending on software priorities)
 *      5) aknowledge the i8259s',
 *      6) demask the processor,
 *      7) call the application handler
 *
 * As a result the hdl function provided
 *
 *      a) can perfectly be written is C,
 *      b) may also well directly call the part of the RTEMS API that can be
 *      used from interrupt level,
 *      c) It only responsible for handling the jobs that need to be done at
 *      the device level including (aknowledging/re-enabling the interrupt at
 *      device level, getting the data,...)
 *
 *      When returning from the function, the following will be performed by
 *      the RTEMS irq epilogue :
 *
 *      1) masks the interrupts again,
 *      2) restore the original i8259s' interrupt masks
 *      3) switch back on the orinal stack if needed,
 *      4) perform rescheduling when necessary,
 *      5) restore the C scratch registers...
 *      6) restore initial execution flow
 *
 */
int BSP_install_rtems_irq_handler       (const rtems_irq_connect_data*);
/*
 * function to get the current RTEMS irq handler for ptr->name. It enables to
 * define hanlder chain...
 */
int BSP_get_current_rtems_irq_handler   (rtems_irq_connect_data* ptr);
/*
 * function to get disconnect the RTEMS irq handler for ptr->name.
 * This function checks that the value given is the current one for safety reason.
 * The user can use the previous function to get it.
 */
int BSP_remove_rtems_irq_handler        (const rtems_irq_connect_data*);


/*
 * ----------------- RTEMS Shared Irq Handler Mngt Routines ----------------
 */
#ifdef BSP_SHARED_HANDLER_SUPPORT
int BSP_install_rtems_shared_irq_handler  (const rtems_irq_connect_data*);
#endif

/*
 * ----------------- RTEMS Global Irq Handler Mngt Routines ----------------
 */
/*
 * (Re) Initialize the RTEMS interrupt management.
 *
 * The result of calling this function will be the same as if each individual
 * handler (config->irqHdlTbl[i].hdl)  different from "config->defaultEntry.hdl"
 * has been individualy connected via
 *      BSP_install_rtems_irq_handler(&config->irqHdlTbl[i])
 * And each handler currently equal to config->defaultEntry.hdl
 * has been previously disconnected via
 *       BSP_remove_rtems_irq_handler (&config->irqHdlTbl[i])
 *
 * This is to say that all information given will be used and not just
 * only the space.
 *
 * CAUTION : the various table address contained in config will be used
 *           directly by the interrupt mangement code in order to save
 *           data size so they must stay valid after the call => they should
 *           not be modified or declared on a stack.
 */

int BSP_rtems_irq_mngt_set(rtems_irq_global_settings* config);

/*
 * (Re) get info on current RTEMS interrupt management.
 */
int BSP_rtems_irq_mngt_get(rtems_irq_global_settings**);

#ifdef __cplusplus
}
#endif

#endif /* ASM */
#endif /* _RTEMS_IRQ_H */
