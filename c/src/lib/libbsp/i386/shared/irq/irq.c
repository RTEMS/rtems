/* irq.c
 *
 *  This file contains the implementation of the function described in irq.h
 *
 *  CopyRight (C) 1998 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */


#include <bsp.h>
#include <irq.h>
#include <rtems/score/thread.h>
#include <rtems/score/apiext.h>

/*
 * pointer to the mask representing the additionnal irq vectors
 * that must be disabled when a particular entry is activated.
 * They will be dynamically computed from teh prioruty table given
 * in BSP_rtems_irq_mngt_set();
 * CAUTION : this table is accessed directly by interrupt routine
 * 	     prologue.
 */
rtems_i8259_masks 	irq_mask_or_tbl[BSP_IRQ_LINES_NUMBER];

/*
 * Copy of data given via initial BSP_rtems_irq_mngt_set() for
 * the sake of efficiency.
 * CAUTION : this table is accessed directly by interrupt routine
 * 	     prologue.
 */
rtems_irq_hdl		current_irq[BSP_IRQ_LINES_NUMBER];
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
/*-------------------------------------------------------------------------+
| Cache for 1st and 2nd PIC IRQ line's status (enabled or disabled) register.
+--------------------------------------------------------------------------*/
/*
 * lower byte is interrupt mask on the master PIC.
 * while upper bits are interrupt on the slave PIC.
 * This cache is initialized in ldseg.s
 */
rtems_i8259_masks i8259s_cache = 0xFFFB;

/*-------------------------------------------------------------------------+
|         Function:  BSP_irq_disable_at_i8259s
|      Description: Mask IRQ line in appropriate PIC chip.
| Global Variables: i8259s_cache
|        Arguments: vector_offset - number of IRQ line to mask.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
int BSP_irq_disable_at_i8259s    (const rtems_irq_symbolic_name irqLine)
{
  unsigned short mask;
  unsigned int 	level;

  if ( ((int)irqLine < BSP_LOWEST_OFFSET) ||
       ((int)irqLine > BSP_MAX_OFFSET )
       )
    return 1;
  
  _CPU_ISR_Disable(level);
  
  mask = 1 << irqLine;
  i8259s_cache |= mask;
  
  if (irqLine < 8)
  {
    outport_byte(PIC_MASTER_IMR_IO_PORT, i8259s_cache & 0xff);
  }
  else
  {
    outport_byte(PIC_SLAVE_IMR_IO_PORT, ((i8259s_cache & 0xff00) >> 8));
  }
  _CPU_ISR_Enable (level);

  return 0;
} 

/*-------------------------------------------------------------------------+
|         Function:  BSP_irq_enable_at_i8259s
|      Description: Unmask IRQ line in appropriate PIC chip.
| Global Variables: i8259s_cache
|        Arguments: irqLine - number of IRQ line to mask.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
int BSP_irq_enable_at_i8259s    (const rtems_irq_symbolic_name irqLine)
{
  unsigned short mask;
  unsigned int 	level;

  if ( ((int)irqLine < BSP_LOWEST_OFFSET) ||
       ((int)irqLine > BSP_MAX_OFFSET )
       )
    return 1;

  _CPU_ISR_Disable(level);
  
  mask = ~(1 << irqLine);
  i8259s_cache &= mask;
  
  if (irqLine < 8)
  {
    outport_byte(PIC_MASTER_IMR_IO_PORT, i8259s_cache & 0xff);
  }
  else
  {
    outport_byte(PIC_SLAVE_IMR_IO_PORT, ((i8259s_cache & 0xff00) >> 8));
  }
  _CPU_ISR_Enable (level);

  return 0;
} /* mask_irq */

int BSP_irq_enabled_at_i8259s        	(const rtems_irq_symbolic_name irqLine)
{
  unsigned short mask;

  if ( ((int)irqLine < BSP_LOWEST_OFFSET) ||
       ((int)irqLine > BSP_MAX_OFFSET )
     )
    return 1;

  mask = (1 << irqLine);
  return  (~(i8259s_cache & mask));
}
  

/*-------------------------------------------------------------------------+
|         Function: BSP_irq_ack_at_i8259s
|      Description: Signal generic End Of Interrupt (EOI) to appropriate PIC.
| Global Variables: None.
|        Arguments: irqLine - number of IRQ line to acknowledge.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
int BSP_irq_ack_at_i8259s  	(const rtems_irq_symbolic_name irqLine)
{
  if ( ((int)irqLine < BSP_LOWEST_OFFSET) ||
       ((int)irqLine > BSP_MAX_OFFSET )
       )
    return 1;

  if (irqLine >= 8) {
   outport_byte(PIC_SLAVE_COMMAND_IO_PORT, PIC_EOI);
  }
  outport_byte(PIC_MASTER_COMMAND_IO_PORT, PIC_EOI);

  return 0;

} /* ackIRQ */

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
  unsigned int i;
  unsigned int j;
  /*
   * Always mask at least current interrupt to prevent re-entrance
   */
  for (i=0; i < internal_config->irqNb; i++) {
    * ((unsigned short*) &irq_mask_or_tbl[i]) = (1 << i);
    for (j = 0; j < internal_config->irqNb; j++) {
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
 * Caution : this function assumes the variable "internal_config"
 * is already set and that the tables it contains are still valid
 * and accessible.
 */
static void make_copy_of_handlers ()
{
  int i;
  for (i=0; i < internal_config->irqNb; i++) {
    current_irq [i] = internal_config->irqHdlTbl[i].hdl;
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
      return 0;
    }
    /*
     * Check if default handler is actually connected. If not issue an error.
     * You must first get the current handler via i386_get_current_idt_entry
     * and then disconnect it using i386_delete_idt_entry.
     * RATIONALE : to always have the same transition by forcing the user
     * to get the previous handler before accepting to disconnect.
     */
    _CPU_ISR_Disable(level);
    if (rtems_hdl_tbl[irq->name].hdl != default_rtems_entry.hdl) {
	  _CPU_ISR_Enable(level);
      return 0;
    }

    /*
     * store the data provided by user
     */
    rtems_hdl_tbl[irq->name] = *irq;
    /*
     * update table used directly by rtems interrupt prologue
     */
    current_irq [irq->name] = irq->hdl;
    /*
     * Enable interrupt at PIC level
     */
    BSP_irq_enable_at_i8259s (irq->name);
    /*
     * Enable interrupt on device
     */
    irq->on(irq);
    
    _CPU_ISR_Enable(level);

    return 1;
}


int BSP_get_current_rtems_irq_handler	(rtems_irq_connect_data* irq)
{
     unsigned int level;

     if (!isValidInterrupt(irq->name)) {
      return 0;
     }
    _CPU_ISR_Disable(level);
     *irq = rtems_hdl_tbl[irq->name];
    _CPU_ISR_Enable(level);
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
    _CPU_ISR_Disable(level);
    if (rtems_hdl_tbl[irq->name].hdl != irq->hdl) {
      _CPU_ISR_Enable(level);
      return 0;
    }

    /*
     * disable interrupt at PIC level
     */
    BSP_irq_disable_at_i8259s (irq->name);

    /*
     * Disable interrupt on device
     */
    irq->off(irq);

    /*
     * restore the default irq value
     */
    rtems_hdl_tbl[irq->name] = default_rtems_entry;

    current_irq[irq->name] = default_rtems_entry.hdl;
    
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
    compute_i8259_masks_from_prio ();
    make_copy_of_handlers ();

    for (i=0; i < internal_config->irqNb; i++) {
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
    _CPU_ISR_Enable(level);
    return 1;
}

int BSP_rtems_irq_mngt_get(rtems_irq_global_settings** config)
{
    *config = internal_config;
    return 0;
}    

void _ThreadProcessSignalsFromIrq (CPU_Exception_frame* ctx)
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
