/*  irq.c
 *
 *  This file contains the implementation of the function described in irq.h
 *
 *  Copyright (C) 1998, 1999 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Special acknowledgement to Till Straumann <strauman@slac.stanford.edu>
 *  for providing inputs to the IRQ handling and optimization.
 *
 *  Modified and added support for the MVME5500 board
 *  Copyright 2003, 2004, Shuchen Kate Feng <feng1@bnl.gov>,
 *                  NSLS,Brookhaven National Laboratory
 *
 */
  
#include <rtems/system.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <rtems/score/thread.h>
#include <rtems/score/apiext.h>
#include <libcpu/raw_exception.h>
#include <libcpu/io.h>
#include <bsp/vectors.h>

#include <rtems/bspIo.h> /* for printk */
#include "bsp/gtreg.h"

#define HI_INT_CAUSE 0x40000000

/*#define DEBUG*/

int gpp_int_error =0;

/*
 * pointer to the mask representing the additionnal irq vectors
 * that must be disabled when a particular entry is activated.
 * They will be dynamically computed from teh prioruty table given
 * in BSP_rtems_irq_mngt_set();
 * CAUTION : this table is accessed directly by interrupt routine
 * 	     prologue.
 */
static unsigned int irq_prio_maskLO_tbl[BSP_MAIN_IRQ_NUMBER];
static unsigned int irq_prio_maskHI_tbl[BSP_MAIN_IRQ_NUMBER];

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

static unsigned int irqCAUSE[20], irqLOW[20], irqHIGH[20];
static int irqIndex=0;

/*
 * Check if IRQ is a MAIN CPU internal IRQ 
 */
static inline int is_main_irq(const rtems_irq_number irqLine)
{
  return (((int) irqLine <= BSP_MICH_IRQ_MAX_OFFSET) &
	  ((int) irqLine >= BSP_MICL_IRQ_LOWEST_OFFSET)
	 );
}

/*
 * Check if IRQ is a GPP IRQ
 */
static inline int is_gpp_irq(const rtems_irq_number irqLine)
{
  return (((int) irqLine <= BSP_GPP_IRQ_MAX_OFFSET) &
	  ((int) irqLine >= BSP_GPP_IRQ_LOWEST_OFFSET)
	 );
}

/*
 * Check if IRQ is a Porcessor IRQ
 */
static inline int is_processor_irq(const rtems_irq_number irqLine)
{
  return (((int) irqLine <= BSP_PROCESSOR_IRQ_MAX_OFFSET) &
	  ((int) irqLine >= BSP_PROCESSOR_IRQ_LOWEST_OFFSET)
	 );
}

#define GT_GPP_Int1_Cause GT_GPP_Interrupt_Cause+1
#define GT_GPP_Int2_Cause GT_GPP_Interrupt_Cause+2
#define GT_GPP_Int3_Cause GT_GPP_Interrupt_Cause+3

void GT_GPP_IntHandler0()
{

  unsigned  gppCause, irqNum, bitNum;
  int i, found=0;

  gppCause = inb(GT_GPP_Interrupt_Cause) & GT_GPPirq_cache;

  for (i=0; GPP7_0IrqTbl[i]!=-1;i++){
    bitNum =GPP7_0IrqTbl[i];
    if (gppCause & (1<<bitNum)) {
        /* Clear the GPP interrupt cause bit */
      outb( ~(1<<bitNum), GT_GPP_Interrupt_Cause);/* Till Straumann */
        found = 1; 
        irqNum = bitNum+BSP_GPP_IRQ_LOWEST_OFFSET;
        /* call the necessary interrupt handlers */
        if (rtems_hdl_tbl[irqNum].hdl != default_rtems_entry.hdl)
	   rtems_hdl_tbl[irqNum].hdl(rtems_hdl_tbl[irqNum].handle);
        else
	   gpp_int_error= bitNum; /*GPP interrupt bitNum not connected */
    }
  }
  if ( !found) gpp_int_error = 33; /* spurious GPP interrupt  */
}

void GT_GPP_IntHandler1()
{
  unsigned  gppCause, irqNum, bitNum;
  int i, found=0;

  gppCause = inb(GT_GPP_Int1_Cause) & (GT_GPPirq_cache>>8);

  for (i=0; GPP15_8IrqTbl[i]!=-1;i++){
    bitNum =GPP15_8IrqTbl[i];
    if (gppCause & (1<<bitNum)) {
        /* Clear the GPP interrupt cause bit */
        outb( ~(1<<bitNum), GT_GPP_Int1_Cause); /* Till Straumann */
        found = 1; 
        irqNum = bitNum+BSP_GPP8_IRQ_OFFSET;
        /* call the necessary interrupt handlers */
        if (rtems_hdl_tbl[irqNum].hdl != default_rtems_entry.hdl)
	   rtems_hdl_tbl[irqNum].hdl(rtems_hdl_tbl[irqNum].handle);
        else
	   gpp_int_error= bitNum+8; /*GPP interrupt bitNum not connected */
    }
  }
  if ( !found) gpp_int_error = 33; /* spurious GPP interrupt  */
}
void GT_GPP_IntHandler2()
{
  unsigned  gppCause, irqNum, bitNum;
  int i, found=0;

  gppCause = inb(GT_GPP_Int2_Cause) & (GT_GPPirq_cache>>16);

  for (i=0; GPP23_16IrqTbl[i]!=-1;i++){
    bitNum =GPP23_16IrqTbl[i];
    if (gppCause & (1<<bitNum)) {
        /* Clear the GPP interrupt cause bit */
        outb( ~(1<<bitNum), GT_GPP_Int2_Cause);
        found = 1; 
        irqNum = bitNum+BSP_GPP16_IRQ_OFFSET;
        /* call the necessary interrupt handlers */
        if (rtems_hdl_tbl[irqNum].hdl != default_rtems_entry.hdl)
	    rtems_hdl_tbl[irqNum].hdl(rtems_hdl_tbl[irqNum].handle);
        else
	   gpp_int_error= bitNum+16; /*GPP interrupt bitNum not connected */
    }
  }
  if ( !found) gpp_int_error = 33; /* spurious GPP interrupt  */
}

void GT_GPP_IntHandler3()
{
  unsigned  gppCause, irqNum, bitNum;
  int i, found=0;

  gppCause = inb(GT_GPP_Int3_Cause) & (GT_GPPirq_cache>>24);

  for (i=0; GPP31_24IrqTbl[i]!=-1;i++){
    bitNum=GPP31_24IrqTbl[i];
    if (gppCause & (1<<bitNum)) {
        /* Clear the GPP interrupt cause bit */
        outb(~(1<<bitNum), GT_GPP_Int3_Cause);
        found = 1; 
        irqNum = bitNum+BSP_GPP24_IRQ_OFFSET;
        /* call the necessary interrupt handlers */
        if (rtems_hdl_tbl[irqNum].hdl != default_rtems_entry.hdl)
	    rtems_hdl_tbl[irqNum].hdl(rtems_hdl_tbl[irqNum].handle);
        else
	   gpp_int_error= bitNum+24; /*GPP interrupt bitNum not connected */
    }
  }
  if ( !found) gpp_int_error = 33; /* spurious GPP interrupt  */
}

/*
 * ------------------------ RTEMS Irq helper functions ----------------
 */
 
/*
 * Caution : this function assumes the variable "internal_config"
 * is already set and that the tables it contains are still valid
 * and accessible.
 */
static void compute_GT64260int_masks_from_prio ()
{
  int i,j;
  unsigned long long irq_prio_mask=0;

  /*
   * Always mask at least current interrupt to prevent re-entrance
   */
  for (i=0; i <BSP_MAIN_IRQ_NUMBER; i++) {
    irq_prio_mask = (unsigned long long) (1LLU << i);
    for (j = 0; j <BSP_MAIN_IRQ_NUMBER; j++) {
      /*
       * Mask interrupts at GT64260int level that have a lower priority
       * or <Till Straumann> a equal priority. 
       */
      if (internal_config->irqPrioTbl [i] >= internal_config->irqPrioTbl [j]) {
	 irq_prio_mask |= (unsigned long long)(1LLU << j);
      }
    }

    irq_prio_maskLO_tbl[i] = irq_prio_mask & 0xffffffff;
    irq_prio_maskHI_tbl[i] = (irq_prio_mask>>32) & 0xffffffff;
#ifdef DEBUG
    printk("irq_mask_prio_tbl[%d]:0x%8x%8x\n",i,irq_prio_maskHI_tbl[i],
	   irq_prio_maskLO_tbl[i]);
#endif  
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
    rtems_interrupt_level       level;
  
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

    if (is_main_irq(irq->name)) {
      /*
       * Enable (internal ) Main Interrupt Cause Low and High
       */
#ifdef DEBUG_IRQ
      printk("main irq %d\n",irq->name);
#endif
      BSP_enable_main_irq(irq->name);
    }
    
    if (is_gpp_irq(irq->name)) {
      /*
       * Enable (external) GPP[x] interrupt 
       */
      BSP_enable_gpp_irq((int) irq->name);
    }

    if (is_processor_irq(irq->name)) {
      /*
       * Enable exception at processor level
       */
    }
    /*
     * Enable interrupt on device
     
    irq->on(irq);*/
    
    rtems_interrupt_enable(level);

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
    rtems_interrupt_level       level;
  
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
    rtems_interrupt_disable(level);

    if (is_main_irq(irq->name)) {
      /*
       * disable CPU main interrupt
       */
      BSP_disable_main_irq(irq->name);
    }
    if (is_gpp_irq(irq->name)) {
      /*
       * disable external interrupt
       */
      BSP_disable_gpp_irq(irq->name);
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

    rtems_interrupt_enable(level);

    return 1;
}

/*
 * ------------------------ RTEMS Global Irq Handler Mngt Routines ----------------
 */

int BSP_rtems_irq_mngt_set(rtems_irq_global_settings* config)
{
    int                    i;
    rtems_interrupt_level  level;

    /*
     * Store various code accelerators
     */
    internal_config 		= config;
    default_rtems_entry 	= config->defaultEntry;
    rtems_hdl_tbl 		= config->irqHdlTbl;

    rtems_interrupt_disable(level);
    compute_GT64260int_masks_from_prio();

    /*
     * set up internal tables used by rtems interrupt prologue
     */
    /*
     * start with MAIN CPU IRQ
     */
    for (i=BSP_MICL_IRQ_LOWEST_OFFSET; i < BSP_GPP_IRQ_LOWEST_OFFSET ; i++) {
      if (rtems_hdl_tbl[i].hdl != default_rtems_entry.hdl) {
	BSP_enable_main_irq(i);
	rtems_hdl_tbl[i].on(&rtems_hdl_tbl[i]);
      }
      else {
	rtems_hdl_tbl[i].off(&rtems_hdl_tbl[i]);
	BSP_disable_main_irq(i);
      }
    }
    /*
     * continue with  external IRQ
     */
    for (i=BSP_GPP_IRQ_LOWEST_OFFSET; i<BSP_PROCESSOR_IRQ_LOWEST_OFFSET; i++) {
      if (rtems_hdl_tbl[i].hdl != default_rtems_entry.hdl) {
	BSP_enable_gpp_irq(i);
	rtems_hdl_tbl[i].on(&rtems_hdl_tbl[i]);
      }
      else {
	rtems_hdl_tbl[i].off(&rtems_hdl_tbl[i]);
	BSP_disable_gpp_irq(i);
      }
    }

    /*
     * finish with Processor exceptions handled like IRQ
     */
    for (i=BSP_PROCESSOR_IRQ_LOWEST_OFFSET; i < BSP_PROCESSOR_IRQ_MAX_OFFSET+1; i++) {
      if (rtems_hdl_tbl[i].hdl != default_rtems_entry.hdl) {
	rtems_hdl_tbl[i].on(&rtems_hdl_tbl[i]);
      }
      else {
	rtems_hdl_tbl[i].off(&rtems_hdl_tbl[i]);
      }
    }
    rtems_interrupt_enable(level);
    return 1;
}

int BSP_rtems_irq_mngt_get(rtems_irq_global_settings** config)
{
    *config = internal_config;
    return 0;
}    

int _BSP_vme_bridge_irq = -1;

/*
 * High level IRQ handler called from shared_raw_irq_code_entry
 */
void C_dispatch_irq_handler (CPU_Interrupt_frame *frame, unsigned int excNum)
{
  register unsigned msr;
  register unsigned new_msr;
  unsigned mainCause[2];
  register unsigned selectCause;
  register unsigned oldMask[2]={0,0};
  unsigned i, regNum, irq, bitNum, startIrqNum=0;

  if (excNum == ASM_DEC_VECTOR) {
    _CPU_MSR_GET(msr);
    new_msr = msr | MSR_EE;
    _CPU_MSR_SET(new_msr);
    
    rtems_hdl_tbl[BSP_DECREMENTER].hdl(rtems_hdl_tbl[BSP_DECREMENTER].handle);

    _CPU_MSR_SET(msr);
    return;
    
  }
  selectCause = inl( GT_CPU_SEL_CAUSE); 
  if (selectCause & HI_INT_CAUSE ) {
    mainCause[1]= selectCause & inl(GT_CPU_INT_MASK_HI);
    startIrqNum=32;
  }
  else {
    mainCause[0] =inl(GT_MAIN_INT_CAUSE_LO)&inl(GT_CPU_INT_MASK_LO);
    mainCause[1] =inl(GT_MAIN_INT_CAUSE_HI)&inl(GT_CPU_INT_MASK_HI);
  }

#if 0
  /* very bad practice to put printk here, use only if for debug */
  printk("main 0 %x, main 1 %x \n", mainCause[0],mainCause[1]);
#endif
  oldMask[0]= GT_MAINirqLO_cache;
  oldMask[1]= GT_MAINirqHI_cache;

  for (i=0;mainIrqTbl[i]!=-1;i++) {
    irq=mainIrqTbl[i];
    if ( irq < startIrqNum ) continue;
    regNum = irq/32;
    bitNum = irq % 32;
    if ( mainCause[regNum] & (1<<bitNum)) {
      GT_MAINirqLO_cache=oldMask[0]&(~irq_prio_maskLO_tbl[irq]);
      outl(GT_MAINirqLO_cache, GT_CPU_INT_MASK_LO);
      __asm __volatile("sync");
      GT_MAINirqHI_cache=oldMask[1]&(~irq_prio_maskHI_tbl[irq]);
      outl(GT_MAINirqHI_cache, GT_CPU_INT_MASK_HI);
      __asm __volatile("sync");

      /* <skf> It seems that reading back is necessary to ensure the 
       * interrupt mask updated. Otherwise, spurious interrupt will
       * happen.  However, I do not want to use "while loop" to risk 
       * the CPU stuck.  I wound rather keep track of the interrupt
       * mask if not updated.
       */
      if (((irqLOW[irqIndex]= inl(GT_CPU_INT_MASK_LO))!=GT_MAINirqLO_cache)||
          ((irqHIGH[irqIndex]= inl(GT_CPU_INT_MASK_HI))!=GT_MAINirqHI_cache)){
         irqIndex++;
         irqIndex %=20;
         irqCAUSE[irqIndex] = irq;
      }
      _CPU_MSR_GET(msr);
      new_msr = msr | MSR_EE;
      _CPU_MSR_SET(new_msr);
      rtems_hdl_tbl[irq].hdl(rtems_hdl_tbl[irq].handle);
      _CPU_MSR_SET(msr);
      break;
    }
  }
  GT_MAINirqLO_cache=oldMask[0];
  outl(GT_MAINirqLO_cache, GT_CPU_INT_MASK_LO);
  GT_MAINirqHI_cache=oldMask[1];
  outl(GT_MAINirqHI_cache, GT_CPU_INT_MASK_HI);
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

void BSP_printIRQMask()
{
  int i;

  for (i=0; i< 20; i++)
    printk("IRQ%d : 0x%x %x \n", irqCAUSE[i], irqHIGH[i],irqLOW[i]);
}
