/*
 *
 *  This file contains the implementation of the function described in irq.h
 *
 *  Copyright (C) 1998, 1999 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */
  
#include <rtems/system.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/VME.h>
#include <bsp/openpic.h>
#include <rtems/score/thread.h>
#include <rtems/score/apiext.h>
#include <libcpu/raw_exception.h>
#include <bsp/vectors.h>

#include <rtems/bspIo.h> /* for printk */
#define RAVEN_INTR_ACK_REG 0xfeff0030

/*
 * pointer to the mask representing the additionnal irq vectors
 * that must be disabled when a particular entry is activated.
 * They will be dynamically computed from teh prioruty table given
 * in BSP_rtems_irq_mngt_set();
 * CAUTION : this table is accessed directly by interrupt routine
 * 	     prologue.
 */
rtems_i8259_masks 	irq_mask_or_tbl[BSP_IRQ_NUMBER];
/*
 * default handler connected on each irq after bsp initialization
 */
static rtems_irq_connect_data	default_rtems_entry;

/*
 * location used to store initial tables used for interrupt
 * management.
 */
static rtems_irq_global_settings* 	internal_config;
static rtems_irq_connect_data*		rtems_hdl_tbl;

/*
 * Check if IRQ is an ISA IRQ
 */
static inline int is_isa_irq(const rtems_irq_symbolic_name irqLine)
{
  return (((int) irqLine <= BSP_ISA_IRQ_MAX_OFFSET) &
	  ((int) irqLine >= BSP_ISA_IRQ_LOWEST_OFFSET)
	 );
}

/*
 * Check if IRQ is an OPENPIC IRQ
 */
static inline int is_pci_irq(const rtems_irq_symbolic_name irqLine)
{
  return (((int) irqLine <= BSP_PCI_IRQ_MAX_OFFSET) &
	  ((int) irqLine >= BSP_PCI_IRQ_LOWEST_OFFSET)
	 );
}

/*
 * Check if IRQ is a Porcessor IRQ
 */
static inline int is_processor_irq(const rtems_irq_symbolic_name irqLine)
{
  return (((int) irqLine <= BSP_PROCESSOR_IRQ_MAX_OFFSET) &
	  ((int) irqLine >= BSP_PROCESSOR_IRQ_LOWEST_OFFSET)
	 );
}


/*
 * ------------------------ RTEMS Irq helper functions ----------------
 */
 
/*
 * Caution : this function assumes the variable "internal_config"
 * is already set and that the tables it contains are still valid
 * and accessible.
 */
static void compute_i8259_masks_from_prio ()
{
  int i;
  int j;
  /*
   * Always mask at least current interrupt to prevent re-entrance
   */
  for (i=BSP_ISA_IRQ_LOWEST_OFFSET; i < BSP_ISA_IRQ_LOWEST_OFFSET + BSP_ISA_IRQ_NUMBER; i++) {
    * ((unsigned short*) &irq_mask_or_tbl[i]) = (1 << i);
    for (j = BSP_ISA_IRQ_LOWEST_OFFSET; j < BSP_ISA_IRQ_LOWEST_OFFSET + BSP_ISA_IRQ_NUMBER; j++) {
      /*
       * Mask interrupts at i8259 level that have a lower priority
       */
      if (internal_config->irqPrioTbl [i] > internal_config->irqPrioTbl [j]) {
	* ((unsigned short*) &irq_mask_or_tbl[i]) |= (1 << j);
      }
    }
  }
}

/*
 * This function check that the value given for the irq line
 * is valid.
 */

static int isValidInterrupt(int irq)
{
  if ( (irq < BSP_LOWEST_OFFSET) || (irq > BSP_MAX_OFFSET))
    return 0;
  return 1;
}

/*
 * ------------------------ RTEMS Single Irq Handler Mngt Routines ----------------
 */

int BSP_install_rtems_irq_handler  (const rtems_irq_connect_data* irq)
{
    unsigned int level;
  
    if (!isValidInterrupt(irq->name)) {
      printk("Invalid interrupt vector %i\n",irq->name);
      return 0;
    }
    /*
     * Check if default handler is actually connected. If not issue an error.
     * You must first get the current handler via i386_get_current_idt_entry
     * and then disconnect it using i386_delete_idt_entry.
     * RATIONALE : to always have the same transition by forcing the user
     * to get the previous handler before accepting to disconnect.
     */
    if (rtems_hdl_tbl[irq->name].hdl != default_rtems_entry.hdl) {
      printk("IRQ vector %i already connected\n",irq->name);
      return 0;
    }
    _CPU_ISR_Disable(level);

    /*
     * store the data provided by user
     */
    rtems_hdl_tbl[irq->name] = *irq;
    
    if (is_isa_irq(irq->name)) {
      /*
       * Enable interrupt at PIC level
       */
      BSP_irq_enable_at_i8259s (irq->name);
    }
    
    if (is_pci_irq(irq->name)) {
      /*
       * Enable interrupt at OPENPIC level
       */
      openpic_enable_irq ((int) irq->name - BSP_PCI_IRQ_LOWEST_OFFSET);
    }

    if (is_processor_irq(irq->name)) {
      /*
       * Enable exception at processor level
       */
    }
    /*
     * Enable interrupt on device
     */
    irq->on(irq);
    
    _CPU_ISR_Enable(level);

    return 1;
}


int BSP_get_current_rtems_irq_handler	(rtems_irq_connect_data* irq)
{
     if (!isValidInterrupt(irq->name)) {
      return 0;
     }
     *irq = rtems_hdl_tbl[irq->name];
     return 1;
}

int BSP_remove_rtems_irq_handler  (const rtems_irq_connect_data* irq)
{
    unsigned int level;
  
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
    if (rtems_hdl_tbl[irq->name].hdl != irq->hdl) {
      return 0;
    }
    _CPU_ISR_Disable(level);

    if (is_isa_irq(irq->name)) {
      /*
       * disable interrupt at PIC level
       */
      BSP_irq_disable_at_i8259s (irq->name);
    }
    if (is_pci_irq(irq->name)) {
      /*
       * disable interrupt at OPENPIC level
       */
      openpic_disable_irq ((int) irq->name - BSP_PCI_IRQ_LOWEST_OFFSET);
    }
    if (is_processor_irq(irq->name)) {
      /*
       * disable exception at processor level
       */
    }    

    /*
     * Disable interrupt on device
     */
    irq->off(irq);

    /*
     * restore the default irq value
     */
    rtems_hdl_tbl[irq->name] = default_rtems_entry;

    _CPU_ISR_Enable(level);

    return 1;
}

/*
 * ------------------------ RTEMS Global Irq Handler Mngt Routines ----------------
 */

int BSP_rtems_irq_mngt_set(rtems_irq_global_settings* config)
{
    int i;
    unsigned int level;
   /*
    * Store various code accelerators
    */
    internal_config 		= config;
    default_rtems_entry 	= config->defaultEntry;
    rtems_hdl_tbl 		= config->irqHdlTbl;

    _CPU_ISR_Disable(level);
    /*
     * set up internal tables used by rtems interrupt prologue
     */
    /*
     * start with ISA IRQ
     */
    compute_i8259_masks_from_prio ();

    for (i=BSP_ISA_IRQ_LOWEST_OFFSET; i < BSP_ISA_IRQ_LOWEST_OFFSET + BSP_ISA_IRQ_NUMBER; i++) {
      if (rtems_hdl_tbl[i].hdl != default_rtems_entry.hdl) {
	BSP_irq_enable_at_i8259s (i);
	rtems_hdl_tbl[i].on(&rtems_hdl_tbl[i]);
      }
      else {
	rtems_hdl_tbl[i].off(&rtems_hdl_tbl[i]);
	BSP_irq_disable_at_i8259s (i);
      }
    }
    /*
     * must enable slave pic anyway
     */
    BSP_irq_enable_at_i8259s (2);
    /*
     * continue with PCI IRQ
     */
    for (i=BSP_PCI_IRQ_LOWEST_OFFSET; i < BSP_PCI_IRQ_LOWEST_OFFSET + BSP_PCI_IRQ_NUMBER ; i++) {
      openpic_set_priority(0, internal_config->irqPrioTbl [i]);
      if (rtems_hdl_tbl[i].hdl != default_rtems_entry.hdl) {
	openpic_enable_irq ((int) i - BSP_PCI_IRQ_LOWEST_OFFSET);
	rtems_hdl_tbl[i].on(&rtems_hdl_tbl[i]);
      }
      else {
	rtems_hdl_tbl[i].off(&rtems_hdl_tbl[i]);
	openpic_disable_irq ((int) i - BSP_PCI_IRQ_LOWEST_OFFSET);
      }
    }
    /*
     * Must enable PCI/ISA bridge IRQ
     */
    openpic_enable_irq (0);
    /*
     * finish with Processor exceptions handled like IRQ
     */
    for (i=BSP_PROCESSOR_IRQ_LOWEST_OFFSET; i < BSP_PROCESSOR_IRQ_LOWEST_OFFSET + BSP_PROCESSOR_IRQ_NUMBER; i++) {
      if (rtems_hdl_tbl[i].hdl != default_rtems_entry.hdl) {
	rtems_hdl_tbl[i].on(&rtems_hdl_tbl[i]);
      }
      else {
	rtems_hdl_tbl[i].off(&rtems_hdl_tbl[i]);
      }
    }
    _CPU_ISR_Enable(level);
    return 1;
}

int BSP_rtems_irq_mngt_get(rtems_irq_global_settings** config)
{
    *config = internal_config;
    return 0;
}    

static unsigned spuriousIntr = 0;
/*
 * High level IRQ handler called from shared_raw_irq_code_entry
 */
void C_dispatch_irq_handler (CPU_Interrupt_frame *frame, unsigned int excNum)
{
  register unsigned int irq;
  register unsigned isaIntr;                  /* boolean */
  register unsigned oldMask = 0;	      /* old isa pic masks */
  register unsigned newMask;                  /* new isa pic masks */
  register unsigned msr;
  register unsigned new_msr;


  if (excNum == ASM_DEC_VECTOR) {
    _CPU_MSR_GET(msr);
    new_msr = msr | MSR_EE;
    _CPU_MSR_SET(new_msr);
    
    rtems_hdl_tbl[BSP_DECREMENTER].hdl();

    _CPU_MSR_SET(msr);
    return;
    
  }
  irq = openpic_irq(0);
  if (irq == OPENPIC_VEC_SPURIOUS) {
    ++spuriousIntr;
    return;
  }
  isaIntr = (irq == BSP_PCI_ISA_BRIDGE_IRQ);
  if (isaIntr)  {
    /*
     * Acknowledge and read 8259 vector
     */
    irq = (unsigned int) (*(unsigned char *) RAVEN_INTR_ACK_REG);
    /*
     * store current PIC mask
     */
    oldMask = i8259s_cache;
    newMask = oldMask | irq_mask_or_tbl [irq];
    i8259s_cache = newMask;
    outport_byte(PIC_MASTER_IMR_IO_PORT, i8259s_cache & 0xff);
    outport_byte(PIC_SLAVE_IMR_IO_PORT, ((i8259s_cache & 0xff00) >> 8));
    BSP_irq_ack_at_i8259s (irq);
    openpic_eoi(0);
  }
  _CPU_MSR_GET(msr);
  new_msr = msr | MSR_EE;
  _CPU_MSR_SET(new_msr);
    
  rtems_hdl_tbl[irq].hdl();

  _CPU_MSR_SET(msr);

  if (isaIntr)  {
    i8259s_cache = oldMask;
    outport_byte(PIC_MASTER_IMR_IO_PORT, i8259s_cache & 0xff);
    outport_byte(PIC_SLAVE_IMR_IO_PORT, ((i8259s_cache & 0xff00) >> 8));
  }
  else {
#ifdef BSP_PCI_VME_DRIVER_DOES_EOI
	/* leave it to the VME bridge driver to do EOI, so
     * it can re-enable the openpic while handling
     * VME interrupts (-> VME priorities in software)
	 */
	if (BSP_PCI_VME_BRIDGE_IRQ!=irq)
#endif
    		openpic_eoi(0);
  }
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
