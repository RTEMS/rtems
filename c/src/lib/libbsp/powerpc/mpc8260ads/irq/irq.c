/*
 *
 *  This file contains the implementation of the function described in irq.h
 *
 *  Copyright (C) 1998, 1999 valette@crf.canon.fr
 *
 *  Modified for mpc8260 Andy Dachs <a.dachs@sstl.co.uk>
 *  Surrey Satellite Technology Limited, 2000
 *  Nested exception handlers not working yet.
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */
  
#include <bsp.h>
#include <bsp/irq.h>
#include <rtems/score/thread.h>
#include <rtems/score/apiext.h>
#include <libcpu/raw_exception.h>
#include <bsp/vectors.h>
#include <libcpu/cpu.h>
/*#include <bsp/8xx_immap.h>*/
#include <mpc8260.h>
/*#include <bsp/commproc.h>*/

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
 * bit in the SIU mask registers (PPC bit numbering) that should
 * be set to enable the relevant interrupt
 *
 */
const static unsigned int SIU_MaskBit[BSP_CPM_IRQ_NUMBER] =
{
     63, 48, 49, 50,  /* err,   i2c,   spi,   rtt   */
     51, 52, 53, 54,  /* smc1,  smc2,  idma1, idma2 */
     55, 56, 57, 63,  /* idma3, idma4, sdma,  -     */
     59, 60, 61, 62,  /* tmr1,  tmr2,  tmr3,  tmr4  */
     29, 30, 63, 17,  /* pit,   tmcnt, -,     irq1  */
     18, 19, 20, 21,  /* irq2,  irq3,  irq4,  irq5  */
     22, 23, 63, 63,  /* irq6,  irq7,  -,     -     */
     63, 63, 63, 63,  /* -,     -,     -,     -     */
     32, 33, 34, 35,  /* fcc1,  fcc2,  fcc3,  -     */
     36, 37, 38, 39,  /* mcc1,  mcc2,  -,     -     */
     40, 41, 42, 43,  /* scc1,  scc2,  scc3,  scc4  */
     44, 45, 46, 47,  /* -,     -,     -,     -     */
     0,  1,   2,  3,  /* pc0,   pc1,   pc2,   pc3   */
     4,  5,   6,  7,  /* pc4,   pc5,   pc6,   pc7   */
     8,  9,  10, 11,  /* pc8,   pc9,   pc10,  pc11  */
     12, 13, 14, 15   /* pc12,  pc13,  pc14,  pc15  */
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
  if ( (irq < BSP_LOWEST_OFFSET) || (irq > BSP_MAX_OFFSET) )
    return 0;
  return 1;
}

int BSP_irq_enable_at_cpm(const rtems_irq_symbolic_name irqLine)
{
  int cpm_irq_index;

  if (!is_cpm_irq(irqLine))
    return 1;

  cpm_irq_index = ((int) (irqLine) - BSP_CPM_IRQ_LOWEST_OFFSET);


  if( SIU_MaskBit[cpm_irq_index] < 32 )
     m8260.simr_h |= (0x80000000 >> SIU_MaskBit[cpm_irq_index]);
  else
     m8260.simr_l |= (0x80000000 >> (SIU_MaskBit[cpm_irq_index]-32));

  return 0;
}

int BSP_irq_disable_at_cpm(const rtems_irq_symbolic_name irqLine)
{
  int cpm_irq_index;
  
  if (!is_cpm_irq(irqLine))
    return 1;
  
  cpm_irq_index = ((int) (irqLine) - BSP_CPM_IRQ_LOWEST_OFFSET);

  if( SIU_MaskBit[cpm_irq_index] < 32 )
     m8260.simr_h &= ~(0x80000000 >> SIU_MaskBit[cpm_irq_index]);
  else
     m8260.simr_l &= ~(0x80000000 >> (SIU_MaskBit[cpm_irq_index]-32));


  return 0;
}

int BSP_irq_enabled_at_cpm(const rtems_irq_symbolic_name irqLine)
{
  int cpm_irq_index;
  
  if (!is_cpm_irq(irqLine))
    return 0;
  
  cpm_irq_index = ((int) (irqLine) - BSP_CPM_IRQ_LOWEST_OFFSET);

  if( SIU_MaskBit[cpm_irq_index] < 32 )
     return m8260.simr_h & (0x80000000 >> SIU_MaskBit[cpm_irq_index]);
  else
     return m8260.simr_l & (0x80000000 >> (SIU_MaskBit[cpm_irq_index]-32));
}


/*
 * ------------------------ RTEMS Single Irq Handler Mngt Routines ----------------
 */

int BSP_install_rtems_irq_handler  (const rtems_irq_connect_data* irq)
{
    unsigned int level;
  
    if (!isValidInterrupt(irq->name)) {
      printk( "not a valid intr\n" ) ;
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
      printk( "Default handler not there\n" );
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

/*
    printk( "Enabled\n" );
*/
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
#if 0
  register unsigned oldMask;		      /* old siu pic masks */
#endif
  register unsigned msr;
  register unsigned new_msr;
#ifdef DISPATCH_HANDLER_STAT
  unsigned loopCounter;
#endif



  /*
   * Handle decrementer interrupt
   */
  if (excNum == ASM_DEC_VECTOR) {

/*
    _BSP_GPLED1_on();
*/
    _CPU_MSR_GET(msr);
    new_msr = msr | MSR_EE;
    _CPU_MSR_SET(new_msr);

    rtems_hdl_tbl[BSP_DECREMENTER].hdl();

    _CPU_MSR_SET(msr);

/*
    _BSP_GPLED1_off();
*/
    return;
  }

  /*
   * Handle external interrupt generated by SIU on PPC core
   */
#ifdef DISPATCH_HANDLER_STAT
  loopCounter = 0;
#endif  
  while (1) {

    if( ((m8260.sipnr_h & m8260.simr_h) | (m8260.sipnr_l & m8260.simr_l)) == 0 ) {
#ifdef DISPATCH_HANDLER_STAT
      if (loopCounter >  maxLoop) maxLoop = loopCounter;
#endif      
      break;
    }

    irq = (m8260.sivec >> 26) + BSP_CPM_IRQ_LOWEST_OFFSET;

/*
    printk( "dispatching %d\n", irq );
*/

    /* Clear pending register */
    if( irq <= BSP_CPM_IRQ_MAX_OFFSET ) {
      if( SIU_MaskBit[irq] < 32 )
        m8260.sipnr_h = (0x80000000 >> SIU_MaskBit[irq]);
      else
        m8260.sipnr_l = (0x80000000 >> (SIU_MaskBit[irq]-32));
    }

/*
    _CPU_MSR_GET(msr);
    new_msr = msr | MSR_EE;
    _CPU_MSR_SET(new_msr);
*/
    rtems_hdl_tbl[irq].hdl();
/*
    _CPU_MSR_SET(msr);
*/



#if 0
    ppc_cached_irq_mask |= (oldMask & ~(SIU_IvectMask[irq]));
    ((volatile immap_t *)IMAP_ADDR)->im_siu_conf.sc_simask = ppc_cached_irq_mask;
#endif
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
