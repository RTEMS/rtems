/*
 *
 *  This file contains the implementation of the function described in irq.h
 *
 *  Copyright (C) 1998, 1999 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */
  
#include <rtems/system.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <rtems/score/thread.h>
#include <rtems/score/apiext.h>
#include <libcpu/raw_exception.h>
#include <bsp/vectors.h>
#include <bsp/8xx_immap.h>
#include <bsp/commproc.h>

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
 * Check if symbolic IRQ name is an SIU IRQ
 */
static inline int is_siu_irq(const rtems_irq_symbolic_name irqLine)
{
  return (((int) irqLine <= BSP_SIU_IRQ_MAX_OFFSET) &
	  ((int) irqLine >= BSP_SIU_IRQ_LOWEST_OFFSET)
	 );
}

/*
 * Check if symbolic IRQ name is an CPM IRQ
 */
static inline int is_cpm_irq(const rtems_irq_symbolic_name irqLine)
{
  return (((int) irqLine <= BSP_CPM_IRQ_MAX_OFFSET) &
	  ((int) irqLine >= BSP_CPM_IRQ_LOWEST_OFFSET)
	 );
}

/*
 * Check if symbolic IRQ name is a Processor IRQ
 */
static inline int is_processor_irq(const rtems_irq_symbolic_name irqLine)
{
  return (((int) irqLine <= BSP_PROCESSOR_IRQ_MAX_OFFSET) &
	  ((int) irqLine >= BSP_PROCESSOR_IRQ_LOWEST_OFFSET)
	 );
}


/*
 * masks used to mask off the interrupts. For exmaple, for ILVL2, the  
 * mask is used to mask off interrupts ILVL2, IRQ3, ILVL3, ... IRQ7    
 * and ILVL7.                                                          
 *
 */
const static unsigned int SIU_IvectMask[BSP_SIU_IRQ_NUMBER] =
{
     /* IRQ0      ILVL0       IRQ1        ILVL1  */
     0x00000000, 0x80000000, 0xC0000000, 0xE0000000,

     /* IRQ2      ILVL2       IRQ3        ILVL3  */
     0xF0000000, 0xF8000000, 0xFC000000, 0xFE000000,

     /* IRQ4      ILVL4       IRQ5        ILVL5  */
     0xFF000000, 0xFF800000, 0xFFC00000, 0xFFE00000,

     /* IRQ6      ILVL6       IRQ7        ILVL7  */
     0xFFF00000, 0xFFF80000, 0xFFFC0000, 0xFFFE0000
};

/*
 * ------------------------ RTEMS Irq helper functions ----------------
 */

/*
 * Caution : this function assumes the variable "internal_config"
 * is already set and that the tables it contains are still valid
 * and accessible.
 */
static void compute_SIU_IvectMask_from_prio ()
{
  /*
   * In theory this is feasible. No time to code it yet. See i386/shared/irq.c
   * for an example based on 8259 controller mask. The actual masks defined
   * correspond to the priorities defined for the SIU in irq_init.c.
   */
}

/*
 * This function check that the value given for the irq line
 * is valid.
 */

static int isValidInterrupt(int irq)
{
  if ( (irq < BSP_LOWEST_OFFSET) || (irq > BSP_MAX_OFFSET) || (irq == BSP_CPM_INTERRUPT) )
    return 0;
  return 1;
}

int BSP_irq_enable_at_cpm(const rtems_irq_symbolic_name irqLine)
{
  int cpm_irq_index;

  if (!is_cpm_irq(irqLine))
    return 1;

  cpm_irq_index = ((int) (irqLine) - BSP_CPM_IRQ_LOWEST_OFFSET);
  ((volatile immap_t *)IMAP_ADDR)->im_cpic.cpic_cimr |= (1 << cpm_irq_index);

  return 0;
}

int BSP_irq_disable_at_cpm(const rtems_irq_symbolic_name irqLine)
{
  int cpm_irq_index;
  
  if (!is_cpm_irq(irqLine))
    return 1;
  
  cpm_irq_index = ((int) (irqLine) - BSP_CPM_IRQ_LOWEST_OFFSET);
  ((volatile immap_t *)IMAP_ADDR)->im_cpic.cpic_cimr &= ~(1 << cpm_irq_index);

  return 0;
}

int BSP_irq_enabled_at_cpm(const rtems_irq_symbolic_name irqLine)
{
  int cpm_irq_index;
  
  if (!is_cpm_irq(irqLine))
    return 0;
  
  cpm_irq_index = ((int) (irqLine) - BSP_CPM_IRQ_LOWEST_OFFSET);
  return (((volatile immap_t *)IMAP_ADDR)->im_cpic.cpic_cimr & (1 << cpm_irq_index));
}

int BSP_irq_enable_at_siu(const rtems_irq_symbolic_name irqLine)
{
  int siu_irq_index;
  
  if (!is_siu_irq(irqLine))
    return 1;

  siu_irq_index = ((int) (irqLine) - BSP_SIU_IRQ_LOWEST_OFFSET);
  ppc_cached_irq_mask |= (1 << (31-siu_irq_index));
  ((volatile immap_t *)IMAP_ADDR)->im_siu_conf.sc_simask = ppc_cached_irq_mask;

  return 0;
}

int BSP_irq_disable_at_siu(const rtems_irq_symbolic_name irqLine)
{
  int siu_irq_index;

  if (!is_siu_irq(irqLine))
    return 1;
  
  siu_irq_index = ((int) (irqLine) - BSP_SIU_IRQ_LOWEST_OFFSET);
  ppc_cached_irq_mask &= ~(1 << (31-siu_irq_index));
  ((volatile immap_t *)IMAP_ADDR)->im_siu_conf.sc_simask = ppc_cached_irq_mask;

  return 0;
}

int BSP_irq_enabled_at_siu     	(const rtems_irq_symbolic_name irqLine)
{
  int siu_irq_index;

  if (!is_siu_irq(irqLine))
    return 0;

  siu_irq_index = ((int) (irqLine) - BSP_SIU_IRQ_LOWEST_OFFSET);
  return ppc_cached_irq_mask & (1 << (31-siu_irq_index));
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
    if (rtems_hdl_tbl[irq->name].hdl != default_rtems_entry.hdl) {
      return 0;
    }

    _CPU_ISR_Disable(level);

    /*
     * store the data provided by user
     */
    rtems_hdl_tbl[irq->name] = *irq;
    
    if (is_cpm_irq(irq->name)) {
      /*
       * Enable interrupt at PIC level
       */
      BSP_irq_enable_at_cpm (irq->name);
    }
    
    if (is_siu_irq(irq->name)) {
      /*
       * Enable interrupt at SIU level
       */
      BSP_irq_enable_at_siu (irq->name);
    }

    if (is_processor_irq(irq->name)) {
      /*
       * Should Enable exception at processor level but not needed.  Will restore
       * EE flags at the end of the routine anyway.
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

    if (is_cpm_irq(irq->name)) {
      /*
       * disable interrupt at PIC level
       */
      BSP_irq_disable_at_cpm (irq->name);
    }
    if (is_siu_irq(irq->name)) {
      /*
       * disable interrupt at OPENPIC level
       */
      BSP_irq_disable_at_siu (irq->name);
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
     * start with CPM IRQ
     */
    for (i=BSP_CPM_IRQ_LOWEST_OFFSET; i < BSP_CPM_IRQ_LOWEST_OFFSET + BSP_CPM_IRQ_NUMBER ; i++) {
      if (rtems_hdl_tbl[i].hdl != default_rtems_entry.hdl) {
	BSP_irq_enable_at_cpm (i);
	rtems_hdl_tbl[i].on(&rtems_hdl_tbl[i]);
      }
      else {
	rtems_hdl_tbl[i].off(&rtems_hdl_tbl[i]);
	BSP_irq_disable_at_cpm (i);
      }
    }

    /*
     * continue with PCI IRQ
     */
    /*
     * set up internal tables used by rtems interrupt prologue
     */
    compute_SIU_IvectMask_from_prio ();

    for (i=BSP_SIU_IRQ_LOWEST_OFFSET; i < BSP_SIU_IRQ_LOWEST_OFFSET + BSP_SIU_IRQ_NUMBER ; i++) {
      if (rtems_hdl_tbl[i].hdl != default_rtems_entry.hdl) {
	BSP_irq_enable_at_siu (i);
	rtems_hdl_tbl[i].on(&rtems_hdl_tbl[i]);
      }
      else {
	rtems_hdl_tbl[i].off(&rtems_hdl_tbl[i]);
	BSP_irq_disable_at_siu (i);
       }
    }
    /*
     * Must enable CPM interrupt on SIU. CPM on SIU Interrupt level has already been
     * set up in BSP_CPM_irq_init.
     */
    ((volatile immap_t *)IMAP_ADDR)->im_cpic.cpic_cicr |= CICR_IEN;
    BSP_irq_enable_at_siu (BSP_CPM_INTERRUPT);
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

#ifdef DISPATCH_HANDLER_STAT
volatile unsigned int maxLoop = 0;
#endif

/*
 * High level IRQ handler called from shared_raw_irq_code_entry
 */
void C_dispatch_irq_handler (CPU_Interrupt_frame *frame, unsigned int excNum)
{
  register unsigned int irq;
  register unsigned cpmIntr;                  /* boolean */
  register unsigned oldMask;		      /* old siu pic masks */
  register unsigned msr;
  register unsigned new_msr;
#ifdef DISPATCH_HANDLER_STAT
  unsigned loopCounter;
#endif
  /*
   * Handle decrementer interrupt
   */
  if (excNum == ASM_DEC_VECTOR) {
    _CPU_MSR_GET(msr);
    new_msr = msr | MSR_EE;
    _CPU_MSR_SET(new_msr);
    
    rtems_hdl_tbl[BSP_DECREMENTER].hdl();

    _CPU_MSR_SET(msr);
    return;
  }
  /*
   * Handle external interrupt generated by SIU on PPC core
   */
#ifdef DISPATCH_HANDLER_STAT
  loopCounter = 0;
#endif  
  while (1) {
    if ((ppc_cached_irq_mask & ((volatile immap_t *)IMAP_ADDR)->im_siu_conf.sc_sipend) == 0) {
#ifdef DISPATCH_HANDLER_STAT
      if (loopCounter >  maxLoop) maxLoop = loopCounter;
#endif      
      break;
    }
    irq = (((volatile immap_t *)IMAP_ADDR)->im_siu_conf.sc_sivec >> 26);
    cpmIntr = (irq == BSP_CPM_INTERRUPT);
    /*
     * Disable the interrupt of the same and lower priority.
     */
    oldMask = ppc_cached_irq_mask;
    ppc_cached_irq_mask = oldMask & SIU_IvectMask[irq];
    ((volatile immap_t *)IMAP_ADDR)->im_siu_conf.sc_simask = ppc_cached_irq_mask;
    /*
     * Acknowledge current interrupt. This has no effect on internal level interrupt.
     */
    ((volatile immap_t *)IMAP_ADDR)->im_siu_conf.sc_sipend = (1 << (31 - irq));
    
    if (cpmIntr)  {
      /*
       * We will reenable the SIU CPM interrupt to allow nesting of CPM interrupt.
       * We must before acknowledege the current irq at CPM level to avoid trigerring
       * the interrupt again. 
       */
      /*
       * Acknowledge and get the vector.
       */
      ((volatile immap_t *)IMAP_ADDR)->im_cpic.cpic_civr = 1;
      irq = (((volatile immap_t *)IMAP_ADDR)->im_cpic.cpic_civr >> 11);
      /*
       * transform IRQ to normalized irq table index.
       */
      irq += BSP_CPM_IRQ_LOWEST_OFFSET;
      /*
       * Unmask CPM interrupt at SIU level
       */
      ppc_cached_irq_mask |= (1 << (31 - BSP_CPM_INTERRUPT));
      ((volatile immap_t *)IMAP_ADDR)->im_siu_conf.sc_simask = ppc_cached_irq_mask;
    }
    _CPU_MSR_GET(msr);
    new_msr = msr | MSR_EE;
    _CPU_MSR_SET(new_msr);
    
    rtems_hdl_tbl[irq].hdl();

    _CPU_MSR_SET(msr);

    if (cpmIntr)  {
      irq -= BSP_CPM_IRQ_LOWEST_OFFSET;
      ((volatile immap_t *)IMAP_ADDR)->im_cpic.cpic_cisr = (1 << irq);
    }
    ppc_cached_irq_mask |= (oldMask & ~(SIU_IvectMask[irq]));
    ((volatile immap_t *)IMAP_ADDR)->im_siu_conf.sc_simask = ppc_cached_irq_mask;
#ifdef DISPATCH_HANDLER_STAT
    ++ loopCounter;
#endif    
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
