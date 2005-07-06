/**
 *  @file irq.h
 *
 *  This include file describe the data structure and the functions implemented
 *  by rtems to write interrupt handlers.
 */
/*
 *  RTEMS GBA BSP
 *
 *  Copyright (c) 2004  Markku Puro <markku.puro@kopteri.net>
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _IRQ_H_
#define _IRQ_H_


#ifdef __cplusplus
extern "C" {
#endif

/*
 * Include some preprocessor value also used by assember code
 */

#define VECTOR_TABLE  (&irq_vector_table[0])

#include <stdint.h>
#include <rtems.h>

extern void default_int_handler();

/*---------------------------------------------------------------------------*
 *  MACROS                                                                   *
 *---------------------------------------------------------------------------*/

#define ENABLE_IRQ()   GBA_REG_IME = 1;
#define DISABLE_IRQ()  GBA_REG_IME = 0;


/*-------------------------------------------------------------------------+
| Constants
+--------------------------------------------------------------------------*/

typedef enum {
  BSP_IRQ_VBLANK    = 0,
  BSP_IRQ_HBLANK    = 1,
  BSP_IRQ_VCOUNTER  = 2,
  BSP_IRQ_TIMER0    = 3,
  BSP_IRQ_TIMER1    = 4,
  BSP_IRQ_TIMER2    = 5,
  BSP_IRQ_TIMER3    = 6,
  BSP_IRQ_SERIAL    = 7,
  BSP_IRQ_DMA0      = 8,
  BSP_IRQ_DMA1      = 9,
  BSP_IRQ_DMA2      = 10,
  BSP_IRQ_DMA3      = 11,
  BSP_IRQ_KEY       = 12,
  BSP_IRQ_CART      = 13,
  BSP_IRQ_NA14      = 14,
  BSP_IRQ_NA15      = 15,
  BSP_MAX_INT       = 16  /**< BSP_MAX_INT <= _irq_max_vector in linkcmds */
} rtems_irq_symbolic_name;

/*
 * Type definition for RTEMS managed interrupts
 */
typedef unsigned char  rtems_irq_level;
typedef unsigned char  rtems_irq_trigger;

extern void        _irq_max_vector;               /**< defined in lincmds    */
extern uint32_t    irq_vector_table[BSP_MAX_INT]; /**< allocated in linkcmds */


struct  __rtems_irq_connect_data__;     /* forward declaratiuon */

typedef void (*rtems_irq_hdl)           (void);
typedef void (*rtems_irq_enable)        (const struct __rtems_irq_connect_data__*);
typedef void (*rtems_irq_disable)       (const struct __rtems_irq_connect_data__*);
typedef int  (*rtems_irq_is_enabled)    (const struct __rtems_irq_connect_data__*);

/** irq connection data structure */
typedef struct __rtems_irq_connect_data__ {
  /**
   * IRQ line
   */
  rtems_irq_symbolic_name       name;
  /**
   * handler. See comment on handler properties below in function prototype.
   */
  rtems_irq_hdl                 hdl;
  /**
   * function for enabling interrupts at device level (ONLY!).
   * The BSP code will automatically enable it at PIC level.
   * RATIONALE : anyway such code has to exist in current driver code.
   * It is usually called immediately AFTER connecting the interrupt handler.
   * RTEMS may well need such a function when restoring normal interrupt
   * processing after a debug session.
   *
   */
    rtems_irq_enable            on;
  /**
   * function for disabling interrupts at device level (ONLY!).
   * The code will disable it at PIC level. RATIONALE : anyway
   * such code has to exist for clean shutdown. It is usually called
   * BEFORE disconnecting the interrupt. RTEMS may well need such
   * a function when disabling normal interrupt processing for
   * a debug session. May well be a NOP function.
   */
  rtems_irq_disable             off;
  /**
   * function enabling to know what interrupt may currently occur
   * if someone manipulates the PIC interrupt mask without care...
   */
    rtems_irq_is_enabled        isOn;
  /**
   * irq priority level
   */
  rtems_irq_level               irqLevel;
  /**
   * Trigger way : Rising or falling edge or High or low level
   */
  rtems_irq_trigger             irqTrigger;
} rtems_irq_connect_data;

/*-------------------------------------------------------------------------+
| Function Prototypes.
+--------------------------------------------------------------------------*/
/*
 * ------------------------ RTEMS Single Irq Handler Mngt Routines ----------------
 */

/**
 * @brief function to initialize the interrupt for a specific BSP
 */
void BSP_rtems_irq_mngt_init();


/**
 * @brief function to connect a particular irq handler.
 * This hanlder will NOT be called directly as the result of the corresponding interrupt.
 * Instead, a RTEMS irq prologue will be called that will :
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
 *      a) can perfectly be written is C,
 *      b) may also well directly call the part of the RTEMS API that can be used
 *      from interrupt level,
 *      c) It only responsible for handling the jobs that need to be done at
 *      the device level including (aknowledging/re-enabling the interrupt at device,
 *      level, getting the data,...)
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

/**
 * function to get the current RTEMS irq handler for ptr->name. It enables to
 * define hanlder chain...
 */
int BSP_get_current_rtems_irq_handler   (rtems_irq_connect_data* ptr);

/**
 * @brief function to get disconnect the RTEMS irq handler for ptr->name.
 * This function checks that the value given is the current one for safety reason.
 * The user can use the previous function to get it.
 */
int BSP_remove_rtems_irq_handler        (const rtems_irq_connect_data*);


#ifdef __cplusplus
}
#endif

#endif /* _IRQ_H_ */
