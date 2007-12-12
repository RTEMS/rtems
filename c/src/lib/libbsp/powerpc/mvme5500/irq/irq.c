/*  irq.c
 *
 *  This file contains the implementation of the function described in irq.h
 *
 *  Copyright (C) 1998, 1999 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  Acknowledgement May 2004 : to Till Straumann <strauman@slac.stanford.edu>
 *  for some inputs.
 *
 *  Copyright 2003, 2004, 2005, 2007  Shuchen Kate Feng <feng1@bnl.gov>,
 *                  NSLS,Brookhaven National Laboratory
 *  1) Modified and added support for the MVME5500 board.
 *  2) The implementation of picIsrTable[] is an original work by the
 *     author to optimize the software IRQ priority scheduling because
 *     Discovery controller does not provide H/W IRQ priority schedule. 
 *     It ensures the fastest/faster interrupt service to the
 *     highest/higher priority IRQ, if pendig.
 *  3) _CPU_MSR_SET() needs RTEMS_COMPILER_MEMORY_BARRIER()
 *
 */
  
#include <rtems/system.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <rtems/score/thread.h>
#include <rtems/score/apiext.h>
#include <libcpu/raw_exception.h>
#include <rtems/rtems/intr.h>
#include <libcpu/io.h>
#include <libcpu/byteorder.h>
#include <bsp/vectors.h>

#include <rtems/bspIo.h> /* for printk */
#include "bsp/gtreg.h"

#define HI_INT_CAUSE 0x40000000

#define MAX_IRQ_LOOP 30

#define EDGE_TRIGGER

/* #define DEBUG_IRQ*/

/*
 * pointer to the mask representing the additionnal irq vectors
 * that must be disabled when a particular entry is activated.
 * They will be dynamically computed from the table given
 * in BSP_rtems_irq_mngt_set();
 * CAUTION : this table is accessed directly by interrupt routine
 * 	     prologue.
 */
static unsigned int BSP_irq_prio_mask_tbl[3][BSP_PIC_IRQ_NUMBER];

/*
 * location used to store initial tables used for interrupt
 * management.
 */
static rtems_irq_global_settings* 	internal_config;
/* handler table (cached copy ) */
static rtems_irq_connect_data*		rtems_hdl_tbl;
/*
 * default handler connected on each irq after bsp initialization
 * (locally cached copy)
 */
void (*default_rtems_hdl)(rtems_irq_hdl_param) = (void(*)(rtems_irq_hdl_param)) -1;


static volatile unsigned  *BSP_irqMask_reg[3];
static volatile unsigned  *BSP_irqCause_reg[3];
static volatile unsigned  BSP_irqMask_cache[3]={0,0,0}; 


static int picIsrTblPtr=0;
static unsigned int GPPIrqInTbl=0;
static unsigned long long MainIrqInTbl=0;

/* 
 * The software developers are forbidden to setup picIsrTable[],
 * as it is a powerful engine for the BSP to find the pending
 * highest priority IRQ at run time.  It ensures the fastest/faster
 * interrupt service to the highest/higher priority IRQ, if pendig.
 *
 * The picIsrTable[96] is updated dynamically at run time
 * based on the priority levels set at BSPirqPrioTable[96],
 * while the BSP_enable_pic_irq(), and BSP_disable_pic_irq()
 * commands are invoked.
 *
 * The picIsrTable[96] lists the enabled CPU main and GPP external interrupt 
 * numbers [0 (lowest)- 95 (highest)] starting from the highest priority
 * one to the lowest priority one. The highest priority interrupt is
 * located at picIsrTable[0], and the lowest priority interrupt is located
 * at picIsrTable[picIsrTblPtr-1].
 *
 *
 */
/* BitNums for Main Interrupt Lo/High Cause and GPP, -1 means invalid bit */ 
static unsigned int picIsrTable[BSP_PIC_IRQ_NUMBER]={  
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1 };


/*
 * Check if IRQ is a MAIN CPU internal IRQ or GPP external IRQ
 */
static inline int is_pic_irq(const rtems_irq_number irqLine)
{
  return (((int) irqLine <= BSP_GPP_IRQ_MAX_OFFSET) &
	  ((int) irqLine >= BSP_MICL_IRQ_LOWEST_OFFSET)
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

static inline unsigned int divIrq32(unsigned irq)
{
  return(irq/32);
}

static inline unsigned int modIrq32(unsigned irq)
{
   return(irq%32);
}

/*
 * ------------------------ RTEMS Irq helper functions ----------------
 */
 
/*
 * Caution : this function assumes the variable "internal_config"
 * is already set and that the tables it contains are still valid
 * and accessible.
 */
static void compute_pic_masks_from_prio(rtems_irq_global_settings *config)
{
  int i,j, k;
  unsigned long long irq_prio_mask=0;

  /*
   * Always mask at least current interrupt to prevent re-entrance
   */
  for (i=0; i <BSP_PIC_IRQ_NUMBER; i++) {
    switch(i) {
      case BSP_MAIN_GPP7_0_IRQ:
      case BSP_MAIN_GPP15_8_IRQ:
      case BSP_MAIN_GPP23_16_IRQ:
      case BSP_MAIN_GPP31_24_IRQ:
	for (k=0; k< 3; k++)
	    BSP_irq_prio_mask_tbl[k][i]=0;

        irq_prio_mask =0;
        break;
      default : 
        irq_prio_mask = (unsigned long long) (1LLU << i);
	break;
    }
    
    if (irq_prio_mask) {
    for (j = 0; j <BSP_MAIN_IRQ_NUMBER; j++) {
        /*
         * Mask interrupts at PIC level that have a lower priority
         * or <Till Straumann> a equal priority. 
         */
        if (config->irqPrioTbl [i] >= config->irqPrioTbl [j])
	   irq_prio_mask |= (unsigned long long)(1LLU << j);
    }
    

    BSP_irq_prio_mask_tbl[0][i] = irq_prio_mask & 0xffffffff;
    BSP_irq_prio_mask_tbl[1][i] = (irq_prio_mask>>32) & 0xffffffff;
#ifdef DEBUG
    printk("irq_mask_prio_tbl[%d]:0x%8x%8x\n",i,BSP_irq_prio_mask_tbl[1][i],
	   BSP_irq_prio_mask_tbl[0][i]);
#endif 

    BSP_irq_prio_mask_tbl[2][i] = 1<<i;
    /* Compute for the GPP priority interrupt mask */
    for (j=BSP_GPP_IRQ_LOWEST_OFFSET; j <BSP_PROCESSOR_IRQ_LOWEST_OFFSET; j++) {      
      if (config->irqPrioTbl [i] >= config->irqPrioTbl [j])
	   BSP_irq_prio_mask_tbl[2][i] |= 1 << (j-BSP_GPP_IRQ_LOWEST_OFFSET); 
    }
    }
  }
}


static void UpdateMainIrqTbl(int irqNum)
{
  int i=0, j, shifted=0;

  switch (irqNum) {
    case BSP_MAIN_GPP7_0_IRQ:
    case BSP_MAIN_GPP15_8_IRQ:
    case BSP_MAIN_GPP23_16_IRQ:
    case BSP_MAIN_GPP31_24_IRQ:  
      return;  /* Do nothing, let GPP take care of it */
      break;
  }
#ifdef SHOW_MORE_INIT_SETTINGS
  unsigned long val2, val1;
#endif

  /* If entry not in table*/
  if ( ((irqNum<BSP_GPP_IRQ_LOWEST_OFFSET) && 
        (!((unsigned long long)(1LLU << irqNum) & MainIrqInTbl))) ||
       ((irqNum>BSP_MICH_IRQ_MAX_OFFSET) && 
        (!(( 1 << (irqNum-BSP_GPP_IRQ_LOWEST_OFFSET)) & GPPIrqInTbl))))
  { 
      while ( picIsrTable[i]!=-1) {
        if (internal_config->irqPrioTbl[irqNum]>internal_config->irqPrioTbl[picIsrTable[i]]) {
          /* all other lower priority entries shifted right */
          for (j=picIsrTblPtr;j>i; j--) 
              picIsrTable[j]=picIsrTable[j-1];
          picIsrTable[i]=irqNum;
          shifted=1;
          break;
       }
       i++;
     }
     if (!shifted) picIsrTable[picIsrTblPtr]=irqNum;
     if (irqNum >BSP_MICH_IRQ_MAX_OFFSET)
        GPPIrqInTbl |= (1<< (irqNum-BSP_GPP_IRQ_LOWEST_OFFSET)); 
     else      
        MainIrqInTbl |= (unsigned long long)(1LLU << irqNum);
     picIsrTblPtr++;
  }
#ifdef SHOW_MORE_INIT_SETTINGS
  val2 = (MainIrqInTbl>>32) & 0xffffffff;
  val1 = MainIrqInTbl&0xffffffff;
  printk("irqNum %d, MainIrqInTbl 0x%x%x\n", irqNum, val2, val1); 
  BSP_printPicIsrTbl();
#endif

}


static void CleanMainIrqTbl(int irqNum)
{
  int i, j;

  switch (irqNum) {
    case BSP_MAIN_GPP7_0_IRQ:
    case BSP_MAIN_GPP15_8_IRQ:
    case BSP_MAIN_GPP23_16_IRQ:
    case BSP_MAIN_GPP31_24_IRQ: 
      return;  /* Do nothing, let GPP take care of it */
      break;  
  }
  if ( ((irqNum<BSP_GPP_IRQ_LOWEST_OFFSET) && 
        ((unsigned long long)(1LLU << irqNum) & MainIrqInTbl)) ||
       ((irqNum>BSP_MICH_IRQ_MAX_OFFSET) && 
        (( 1 << (irqNum-BSP_GPP_IRQ_LOWEST_OFFSET)) & GPPIrqInTbl)))
  { /* If entry in table*/
     for (i=0; i<64; i++) {
       if (picIsrTable[i]==irqNum) {/*remove it from the entry */
          /* all other lower priority entries shifted left */
          for (j=i;j<picIsrTblPtr; j++) 
              picIsrTable[j]=picIsrTable[j+1];
          if (irqNum >BSP_MICH_IRQ_MAX_OFFSET)
            GPPIrqInTbl &= ~(1<< (irqNum-BSP_GPP_IRQ_LOWEST_OFFSET)); 
          else      
            MainIrqInTbl &= ~(1LLU << irqNum);
          picIsrTblPtr--;
          break;
       }
     }
  }
}

void BSP_enable_pic_irq(const rtems_irq_number irqNum)
{
  unsigned bitNum, regNum;
  unsigned int level;

  if ( !is_pic_irq(irqNum) )
  	return;

  bitNum = modIrq32(((unsigned int)irqNum) - BSP_MICL_IRQ_LOWEST_OFFSET);
  regNum = divIrq32(((unsigned int)irqNum) - BSP_MICL_IRQ_LOWEST_OFFSET);

  rtems_interrupt_disable(level); 

  UpdateMainIrqTbl((int) irqNum);
  BSP_irqMask_cache[regNum] |= (1 << bitNum);

  out_le32(BSP_irqMask_reg[regNum], BSP_irqMask_cache[regNum]);
  while (in_le32(BSP_irqMask_reg[regNum]) != BSP_irqMask_cache[regNum]);

  rtems_interrupt_enable(level);
}

void BSP_enable_irq_at_pic(const rtems_irq_number irqNum)
{
	BSP_enable_pic_irq(irqNum);
}

int BSP_disable_irq_at_pic(const rtems_irq_number irqNum)
{
  int      rval;
  unsigned bitNum, regNum;
  unsigned int level;

  if ( ! is_pic_irq(irqNum) )
  	return -1;

  bitNum = modIrq32(((unsigned int)irqNum) - BSP_MICL_IRQ_LOWEST_OFFSET);
  regNum = divIrq32(((unsigned int)irqNum) - BSP_MICL_IRQ_LOWEST_OFFSET);

  rtems_interrupt_disable(level); 

  CleanMainIrqTbl((int) irqNum);

  rval = BSP_irqMask_cache[regNum] & (1<<bitNum);

  BSP_irqMask_cache[regNum] &=  ~(1 << bitNum);

  out_le32(BSP_irqMask_reg[regNum], BSP_irqMask_cache[regNum]);
  while (in_le32(BSP_irqMask_reg[regNum]) != BSP_irqMask_cache[regNum]);

  rtems_interrupt_enable(level);

  return rval ? 1 : 0;
}

void BSP_disable_pic_irq(const rtems_irq_number irqNum)
{
	(void)BSP_disable_irq_at_pic(irqNum);
}

int BSP_setup_the_pic(rtems_irq_global_settings *config)  /* adapt the same name as shared/irq */
{
    int i;

	internal_config   = config;
	default_rtems_hdl = config->defaultEntry.hdl;
	rtems_hdl_tbl     = config->irqHdlTbl;

    /* Get ready for discovery BSP */
    BSP_irqMask_reg[0]= (volatile unsigned int *) (GT64260_REG_BASE + GT_CPU_INT_MASK_LO);
    BSP_irqMask_reg[1]= (volatile unsigned int *) (GT64260_REG_BASE + GT_CPU_INT_MASK_HI);
    BSP_irqMask_reg[2]= (volatile unsigned int *) (GT64260_REG_BASE + GT_GPP_Interrupt_Mask);

    BSP_irqCause_reg[0]= (volatile unsigned int *) (GT64260_REG_BASE + GT_MAIN_INT_CAUSE_LO);
    BSP_irqCause_reg[1]= (volatile unsigned int *) (GT64260_REG_BASE + GT_MAIN_INT_CAUSE_HI);
    BSP_irqCause_reg[2]= (volatile unsigned int *) (GT64260_REG_BASE + GT_GPP_Interrupt_Cause);

#ifdef EDGE_TRIGGER

    /* Page 401, Table 598:
     * Comm Unit Arbiter Control register :
     * bit 10:GPP interrupts as level sensitive(1) or edge sensitive(0).
     * We set the GPP interrupts to be edge sensitive.
     * MOTload default is set as level sensitive(1).
     */
    outl((inl(GT_CommUnitArb_Ctrl)& (~(1<<10))), GT_CommUnitArb_Ctrl);
#else
    outl((inl(GT_CommUnitArb_Ctrl)| (1<<10)), GT_CommUnitArb_Ctrl);
#endif

#if 0
    printk("BSP_irqMask_reg[0] = 0x%x, BSP_irqCause_reg[0] 0x%x\n", 
	   in_le32(BSP_irqMask_reg[0]),
	   in_le32(BSP_irqCause_reg[0]));
    printk("BSP_irqMask_reg[1] = 0x%x, BSP_irqCause_reg[1] 0x%x\n", 
	   in_le32(BSP_irqMask_reg[1]),
	   in_le32(BSP_irqCause_reg[1]));
    printk("BSP_irqMask_reg[2] = 0x%x, BSP_irqCause_reg[2] 0x%x\n", 
	   in_le32(BSP_irqMask_reg[2]),
	   in_le32(BSP_irqCause_reg[2]));
#endif

    /* Initialize the interrupt related GT64260 registers */
    for (i=0; i<3; i++) {
      out_le32(BSP_irqCause_reg[i], 0);
      out_le32(BSP_irqMask_reg[i], 0);
    }         
    in_le32(BSP_irqMask_reg[2]);
    compute_pic_masks_from_prio(config);

#if 0
    printk("BSP_irqMask_reg[0] = 0x%x, BSP_irqCause_reg[0] 0x%x\n", 
	   in_le32(BSP_irqMask_reg[0]),
	   in_le32(BSP_irqCause_reg[0]));
    printk("BSP_irqMask_reg[1] = 0x%x, BSP_irqCause_reg[1] 0x%x\n", 
	   in_le32(BSP_irqMask_reg[1]),
	   in_le32(BSP_irqCause_reg[1]));
    printk("BSP_irqMask_reg[2] = 0x%x, BSP_irqCause_reg[2] 0x%x\n", 
	   in_le32(BSP_irqMask_reg[2]),
	   in_le32(BSP_irqCause_reg[2]));
#endif

    return(1);
}

/*
 * This function check that the value given for the irq line
 * is valid.
 */

/*
 * High level IRQ handler called from shared_raw_irq_code_entry
 */

int C_dispatch_irq_handler (BSP_Exception_frame *frame, unsigned int excNum)
{
  unsigned long irqCause[3]={0, 0,0};
  register unsigned long selectCause;
  unsigned oldMask[3]={0,0,0};
  register unsigned i=0, j, irq=0, bitmask=0, group=0;

  if (excNum == ASM_DEC_VECTOR) {

  	bsp_irq_dispatch_list( rtems_hdl_tbl, BSP_DECREMENTER, default_rtems_hdl);

    return 0;
    
  }

  for (j=0; j<3; j++ ) oldMask[j] = BSP_irqMask_cache[j];

  if ((selectCause= in_le32((volatile unsigned *)0xf1000c70)) & HI_INT_CAUSE ){
    irqCause[1] = (selectCause & BSP_irqMask_cache[1]);
    irqCause[2] = in_le32(BSP_irqCause_reg[2]) & BSP_irqMask_cache[2];
  }
  else {
    irqCause[0] = (selectCause & BSP_irqMask_cache[0]);
    if ((irqCause[1] =(in_le32((volatile unsigned *)0xf1000c68)&BSP_irqMask_cache[1])))
       irqCause[2] = in_le32(BSP_irqCause_reg[2]) & BSP_irqMask_cache[2];
  }
  
  while ((irq = picIsrTable[i++])!=-1)
  {
    if (irqCause[group=(irq/32)] && (irqCause[group]&(bitmask=(1<<(irq % 32))))) {
      for (j=0; j<3; j++)
        BSP_irqMask_cache[j] &= (~ BSP_irq_prio_mask_tbl[j][irq]);

      RTEMS_COMPILER_MEMORY_BARRIER();
      out_le32((volatile unsigned *)0xf1000c1c, BSP_irqMask_cache[0]);
      out_le32((volatile unsigned *)0xf1000c6c, BSP_irqMask_cache[1]);
      out_le32((volatile unsigned *)0xf100f10c, BSP_irqMask_cache[2]);
      in_le32((volatile unsigned *)0xf100f10c);

#ifdef EDGE_TRIGGER
      if (irq > BSP_MICH_IRQ_MAX_OFFSET)
         out_le32(BSP_irqCause_reg[2], ~bitmask);/* Till Straumann: Ack the edge triggered GPP IRQ */
#endif

  	  bsp_irq_dispatch_list( rtems_hdl_tbl, irq, default_rtems_hdl);
      
      for (j=0; j<3; j++ ) BSP_irqMask_cache[j] = oldMask[j];
      break;
    }
  }

  out_le32((volatile unsigned *)0xf1000c1c, oldMask[0]);
  out_le32((volatile unsigned *)0xf1000c6c, oldMask[1]);
  out_le32((volatile unsigned *)0xf100f10c, oldMask[2]);
  in_le32((volatile unsigned *)0xf100f10c);

  return 0;
}

/* Only print part of the entries for now */
void BSP_printPicIsrTbl()
{
  int i;

  printk("picIsrTable[12]={");
  for (i=0; i<12; i++)
    printk("%d,", picIsrTable[i]);
  printk("}\n");

  printk("GPPIrqInTbl: 0x%x :\n", GPPIrqInTbl);
}
