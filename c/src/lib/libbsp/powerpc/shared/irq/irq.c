/*
 *
 *  This file contains the PIC-independent implementation of the
 *  functions described in irq.h
 *
 *  Copyright (C) 1998, 1999 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <stdlib.h>

#include <bsp.h>
#include <bsp/irq_supp.h>
#include <rtems/irq.h>
#include <rtems/score/apiext.h>  /* for post ISR signal processing */
#include <libcpu/raw_exception.h>
#include <bsp/vectors.h>
#include <stdlib.h>

#include <rtems/bspIo.h> /* for printk */

extern unsigned int external_exception_vector_prolog_code_size[];
extern void external_exception_vector_prolog_code();
extern unsigned int decrementer_exception_vector_prolog_code_size[];
extern void decrementer_exception_vector_prolog_code();

/*
 * default handler connected on each irq after bsp initialization
 */
static rtems_irq_connect_data  default_rtems_entry;

/*
 * location used to store initial tables used for interrupt
 * management.
 */
static rtems_irq_global_settings*   internal_config;
static rtems_irq_connect_data*    rtems_hdl_tbl;

/*
 * ------------------------ RTEMS Irq helper functions ----------------
 */

/*
 * This function check that the value given for the irq line
 * is valid.
 */

static int isValidInterrupt(int irq)
{
  if ( (irq < internal_config->irqBase) ||
       (irq >= internal_config->irqBase + internal_config->irqNb))
    return 0;
  return 1;
}

/*
 * ------------------- RTEMS Shared Irq Handler Mngt Routines ------------
 */
int BSP_install_rtems_shared_irq_handler  (const rtems_irq_connect_data* irq)
{
  rtems_interrupt_level   level;
  rtems_irq_connect_data* vchain;

  if (!isValidInterrupt(irq->name)) {
    printk("Invalid interrupt vector %d\n",irq->name);
    return 0;
  }

  rtems_interrupt_disable(level);

  if ( (int)rtems_hdl_tbl[irq->name].next_handler  == -1 ) {
    rtems_interrupt_enable(level);
    printk(
      "IRQ vector %d already connected to an unshared handler\n",
      irq->name
    );
    return 0;
  }

   vchain = (rtems_irq_connect_data*)malloc(sizeof(rtems_irq_connect_data));

  /* save off topmost handler */
  vchain[0]= rtems_hdl_tbl[irq->name];

  /*
   * store the data provided by user
   */
  rtems_hdl_tbl[irq->name] = *irq;

  /* link chain to new topmost handler */
  rtems_hdl_tbl[irq->name].next_handler = (void *)vchain;

  /*
   * enable_irq_at_pic is supposed to ignore
   * requests to disable interrupts outside
   * of the range handled by the PIC
   */
  BSP_enable_irq_at_pic(irq->name);

  /*
   * Enable interrupt on device
   */
  if (irq->on)
    irq->on(irq);

  rtems_interrupt_enable(level);

  return 1;
}

/*
 * ------------------- RTEMS Single Irq Handler Mngt Routines ------------
 */

int BSP_install_rtems_irq_handler  (const rtems_irq_connect_data* irq)
{
  rtems_interrupt_level  level;

  if (!isValidInterrupt(irq->name)) {
    printk("Invalid interrupt vector %d\n",irq->name);
    return 0;
  }
  /*
   * Check if default handler is actually connected. If not issue an error.
   * You must first get the current handler via i386_get_current_idt_entry
   * and then disconnect it using i386_delete_idt_entry.
   * RATIONALE : to always have the same transition by forcing the user
   * to get the previous handler before accepting to disconnect.
   */
  rtems_interrupt_disable(level);
  if (rtems_hdl_tbl[irq->name].hdl != default_rtems_entry.hdl) {
    rtems_interrupt_enable(level);
    printk("IRQ vector %d already connected\n",irq->name);
    return 0;
  }

  /*
   * store the data provided by user
   */
  rtems_hdl_tbl[irq->name] = *irq;
  rtems_hdl_tbl[irq->name].next_handler = (void *)-1;

  /*
   * enable_irq_at_pic is supposed to ignore
   * requests to disable interrupts outside
   * of the range handled by the PIC
   */
  BSP_enable_irq_at_pic(irq->name);

  /*
   * Enable interrupt on device
   */
  if (irq->on)
    irq->on(irq);

  rtems_interrupt_enable(level);

  return 1;
}

int BSP_get_current_rtems_irq_handler  (rtems_irq_connect_data* irq)
{
  rtems_interrupt_level       level;

  if (!isValidInterrupt(irq->name)) {
    return 0;
  }
  rtems_interrupt_disable(level);
  *irq = rtems_hdl_tbl[irq->name];
  rtems_interrupt_enable(level);
  return 1;
}

int BSP_remove_rtems_irq_handler  (const rtems_irq_connect_data* irq)
{
  rtems_irq_connect_data *pchain= NULL, *vchain = NULL;
  rtems_interrupt_level   level;

  if (!isValidInterrupt(irq->name)) {
    return 0;
  }
  /*
   * Check if default handler is actually connected. If not issue an error.
   * You must first get the current handler via i386_get_current_idt_entry
   * and then disconnect it using i386_delete_idt_entry.
   * RATIONALE : to always have the same transition by forcing the user
   * to get the previous handler before accepting to disconnect.
   */
  rtems_interrupt_disable(level);
  if (rtems_hdl_tbl[irq->name].hdl != irq->hdl) {
    rtems_interrupt_enable(level);
    return 0;
  }

  if ( (int)rtems_hdl_tbl[irq->name].next_handler != -1 ) {
    int found = 0;

    for( (pchain= NULL, vchain = &rtems_hdl_tbl[irq->name]);
         (vchain->hdl != default_rtems_entry.hdl);
         (pchain= vchain,
          vchain = (rtems_irq_connect_data*)vchain->next_handler) ) {
      if ( vchain->hdl == irq->hdl ) {
        found =  -1;
        break;
      }
    }

    if ( !found ) {
      rtems_interrupt_enable(level);
      return 0;
    }
  } else {
    if (rtems_hdl_tbl[irq->name].hdl != irq->hdl) {
      rtems_interrupt_enable(level);
      return 0;
    }
  }

  /*
   * disable_irq_at_pic is supposed to ignore
   * requests to disable interrupts outside
   * of the range handled by the PIC
   */
  BSP_disable_irq_at_pic(irq->name);

  /*
   * Disable interrupt on device
   */
  if (irq->off)
    irq->off(irq);

  /*
   * restore the default irq value
   */
  if( !vchain ) {
     /* single handler vector... */
     rtems_hdl_tbl[irq->name] = default_rtems_entry;
  } else {
    if ( pchain ) {
       /* non-first handler being removed */
       pchain->next_handler = vchain->next_handler;
    } else {
       /* first handler isn't malloc'ed, so just overwrite it.  Since
        * the contents of vchain are being struct copied, vchain itself
        * goes away
        */
       vchain = vchain->next_handler;
       rtems_hdl_tbl[irq->name]= *vchain;
    }
    free(vchain);
  }

  rtems_interrupt_enable(level);

  return 1;
}

/*
 * RTEMS Global Interrupt Handler Management Routines
 */

int BSP_rtems_irq_mngt_set(rtems_irq_global_settings* config)
{
  int                           i;
  rtems_interrupt_level         level;
  rtems_irq_connect_data*       vchain;
  rtems_raw_except_connect_data vectorDesc;

  /*
   * Store various code accelerators
   */
  internal_config     = config;
  default_rtems_entry   = config->defaultEntry;
  rtems_hdl_tbl     = config->irqHdlTbl;

  rtems_interrupt_disable(level);

  if ( !BSP_setup_the_pic(config) ) {
    printk("PIC setup failed; leaving IRQs OFF\n");
    return 0;
  }

  for ( i = config->irqBase; i < config->irqBase + config->irqNb; i++ ) {
    for( vchain = &rtems_hdl_tbl[i];
         ((int)vchain != -1 && vchain->hdl != default_rtems_entry.hdl); 
         vchain = (rtems_irq_connect_data*)vchain->next_handler ) {
      if (vchain->on)
        vchain->on(vchain);
    }
  }

    rtems_interrupt_enable(level);

  /*
   * We must connect the raw irq handler for the two
   * expected interrupt sources : decrementer and external interrupts.
   */
  vectorDesc.exceptIndex       =  ASM_DEC_VECTOR;
  vectorDesc.hdl.vector        =  ASM_DEC_VECTOR;
  vectorDesc.hdl.raw_hdl       =  decrementer_exception_vector_prolog_code;
  vectorDesc.hdl.raw_hdl_size  = 
       (unsigned) decrementer_exception_vector_prolog_code_size;
  vectorDesc.on     =  0;
  vectorDesc.off    =  0;
  vectorDesc.isOn   =  0;
  if (!ppc_set_exception (&vectorDesc)) {
    BSP_panic("Unable to initialize RTEMS decrementer raw exception\n");
  }

  vectorDesc.exceptIndex       =  ASM_EXT_VECTOR;
  vectorDesc.hdl.vector        =  ASM_EXT_VECTOR;
  vectorDesc.hdl.raw_hdl       =  external_exception_vector_prolog_code;
  vectorDesc.hdl.raw_hdl_size  =
       (unsigned) external_exception_vector_prolog_code_size;
  if (!ppc_set_exception (&vectorDesc)) {
    BSP_panic("Unable to initialize RTEMS external raw exception\n");
  }
  return 1;
}

int BSP_rtems_irq_mngt_get(rtems_irq_global_settings** config)
{
  *config = internal_config;
  return 0;
}

void _ThreadProcessSignalsFromIrq (BSP_Exception_frame* ctx)
{
  /*
   * Process pending signals that have not already been
   * processed by _Thread_Displatch. This happens quite
   * unfrequently : the ISR must have posted an action
   * to the current running thread.
   */
  if ( _Thread_Do_post_task_switch_extension ||
       _Thread_Executing->do_post_task_switch_extension ) {
    _Thread_Executing->do_post_task_switch_extension = FALSE;
    _API_extensions_Run_postswitch();
  }
  /*
   * I plan to process other thread related events here.
   * This will include DEBUG session requested from keyboard...
   */
}
