/* GT64260Int.c - GT64260 Interrupt controller support functions 
 *
 * Copyright 2003, 2004, Brookhaven National Laboratory and
 *                 Shuchen Kate Feng <feng1@bnl.gov>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution.
 *
 */
#include <libcpu/io.h>
#include <rtems/bspIo.h> /* for printk */

#include "bsp/gtreg.h"
#include "bsp/irq.h"

extern rtems_irq_prio BSPirqPrioTable[BSP_MAIN_IRQ_NUMBER];

rtems_GTirq_masks GT_GPPirq_cache=0; 
rtems_GTirq_masks GT_MAINirqLO_cache=0, GT_MAINirqHI_cache=0;

void BSP_GT64260INT_init()
{

    /* Page 401, Table 598:
     * Comm Unit Arbiter Control register :
     * bit 10:GPP interrupts as level sensitive(1) or edge sensitive(0).
     * We set the GPP interrupts to be edge sensitive.
     * MOTload default is set as level sensitive(1).
     */
    outl((inl(GT_CommUnitArb_Ctrl)& (~(1<<10))), GT_CommUnitArb_Ctrl);

    /* Initialize the interrupt related GT64260 registers */
    outl( 0, GT_CPU_INT_MASK_LO);
    outl( 0, GT_CPU_INT_MASK_HI);

    outl( 0, GT_PCI0_INT_MASK_LO);
    outl( 0, GT_PCI0_INT_MASK_HI);


    outl( 0, GT_PCI1_INT_MASK_LO);
    outl( 0, GT_PCI1_INT_MASK_HI);
 
    outl( 0, GT_CPU_INT0_MASK);
    outl( 0, GT_CPU_INT1_MASK);
    outl( 0, GT_CPU_INT2_MASK);
    outl( 0, GT_CPU_INT3_MASK);
    outl(0, GT_GPP_Interrupt_Mask);
    outl( 0, GT_GPP_Value);
    outl( 0, GT_GPP_Interrupt_Cause);
#if 0
    printk("watchdog timer 0x%x\n",inl(0xb410));
#endif
}

static void UpdateMainIrqTbl(int irqNum)
{
  int i=0, j, shifted=0;

#ifdef SHOW_MORE_INIT_SETTINGS
  unsigned long val2, val1;

  val2 = (MainIrqInTbl>>32) & 0xffffffff;
  val1 = MainIrqInTbl&0xffffffff;
  printk("irqNum %d, MainIrqInTbl 0x%x%x\n", irqNum, val2, val1); 
  printMainIrqTbl();
#endif

  /* If entry not in table*/
  if ( !((unsigned long long)(1LLU << irqNum) & MainIrqInTbl)) { 
      while ( mainIrqTbl[i]!=-1) {
        if (BSPirqPrioTable[irqNum]>BSPirqPrioTable[mainIrqTbl[i]]) {
          /* all other lower priority entries shifted right */
          for (j=MainIrqTblPtr;j>i; j--) 
              mainIrqTbl[j]=mainIrqTbl[j-1];
          mainIrqTbl[i]=irqNum;
          shifted=1;
          break;
       }
       i++;
     }
     if (!shifted) mainIrqTbl[MainIrqTblPtr]=irqNum;
     MainIrqInTbl |= (unsigned long long)(1LLU << irqNum);
     MainIrqTblPtr++;
  }
}

static void CleanMainIrqTbl(int irqNum)
{
  int i, j;

  if (((1LLU << irqNum) & MainIrqInTbl)) { /* If entry in table*/
     for (i=0; i<64; i++) {
       if (mainIrqTbl[i]==irqNum) {/*remove it from the entry */
          /* all other lower priority entries shifted left */
          for (j=i;j<MainIrqTblPtr; j++) 
              mainIrqTbl[j]=mainIrqTbl[j+1];
          MainIrqInTbl &= ~(1LLU << irqNum);
          MainIrqTblPtr--;
          break;
       }
     }
  }
}

/***************************************************************************
*
* BSP_enable_main_irq enables the corresponding bit in the low or high
* "main cause cpu int mask register".  
*
*/
void BSP_enable_main_irq(const rtems_irq_number irqNum) 
{
  unsigned              bitNum;
  rtems_interrupt_level level;

  bitNum = ((int)irqNum) - BSP_MICL_IRQ_LOWEST_OFFSET;

  rtems_interrupt_disable(level); 

#if DynamicIrqTbl 
  UpdateMainIrqTbl((int) irqNum);
#endif       
  if (bitNum <32) {
     GT_MAINirqLO_cache |= (1 << bitNum);
     outl(GT_MAINirqLO_cache, GT_CPU_INT_MASK_LO);
  }
  else {
     bitNum-=32;
     GT_MAINirqHI_cache |= (1 << bitNum);
     outl(GT_MAINirqHI_cache, GT_CPU_INT_MASK_HI);
  }
  rtems_interrupt_enable(level);
}

/***************************************************************************
*
* BSP_disable_main_irq disables the corresponding bit in the low or high
* main cause cpu int mask register. 
*
*/
void BSP_disable_main_irq(const rtems_irq_number irqNum) 
{
  unsigned              bitNum;
  rtems_interrupt_level level;

  bitNum = ((int)irqNum) - BSP_MICL_IRQ_LOWEST_OFFSET;

  rtems_interrupt_disable(level);

#if DynamicIrqTbl 
  CleanMainIrqTbl((int) irqNum);
#endif
  if (bitNum <32) {
     GT_MAINirqLO_cache &= ~(1 << bitNum);
     outl(GT_MAINirqLO_cache, GT_CPU_INT_MASK_LO);
  }
  else	{
     bitNum-=32;
     GT_MAINirqHI_cache &= ~(1 << bitNum);
     outl(GT_MAINirqHI_cache, GT_CPU_INT_MASK_HI);
  }
  rtems_interrupt_enable(level);
}

/******************************************************************************
*
* BSP_enable_gpp_irq enables the corresponding bit in the GPP interrupt
* mask register.  The interrupt level is numerically equivalent to the
* corresponding interrupt vector.
*
*/
void BSP_enable_gpp_irq(const rtems_irq_number irqNum) 
{
  unsigned              bitNum;
  unsigned int          mask;
  int                   group;
  int                   bit;
  rtems_interrupt_level level;

  bitNum = ((int)irqNum) - BSP_GPP_IRQ_LOWEST_OFFSET;

  rtems_interrupt_disable(level);

#if DynamicIrqTbl 
  group = bitNum/8;
  if ( !GPPinMainIrqTbl[group])  /* avoid duplicated entry */
     UpdateMainIrqTbl(BSP_MAIN_GPP7_0_IRQ+group);
  bit = bitNum%8;
  GPPinMainIrqTbl[group] |= (1<<bit);
#endif
 
  mask = 1 << bitNum;
  GT_GPPirq_cache |= mask;
  outl(GT_GPPirq_cache, GT_GPP_Interrupt_Mask);
#ifdef SHOW_MORE_INIT_SETTINGS
  printk("enable irqNum %d, bitnum %d \n", irqNum, bitNum);
  printk("GPP mask %d \n", inl(GT_GPP_Interrupt_Mask)); 
#endif

  rtems_interrupt_enable(level);
}

/******************************************************************************
*
* BSP_disable_gpp_irq disables the corresponding bit in the General Purpose
* Port Interrupt.  The interrupt level is numerically equivalent to the
* corresponding interrupt vector.
*
*/
void BSP_disable_gpp_irq(const rtems_irq_number irqNum) 
{
  unsigned              bitNum;
  unsigned int          mask;
  int                   group;
  int                   bit;
  rtems_interrupt_level level;


  bitNum = ((int)irqNum) - BSP_GPP_IRQ_LOWEST_OFFSET;

  rtems_interrupt_disable(level);
#if DynamicIrqTbl 
  group = bitNum/8;
  bit = bitNum%8;
  GPPinMainIrqTbl[group] &= ~(1<<bit);
  if ( !GPPinMainIrqTbl[group])/* If it's really the last one */
     CleanMainIrqTbl(BSP_MAIN_GPP7_0_IRQ+group);
#endif
#ifdef SHOW_MORE_INIT_SETTINGS
  printk("disable irqNum %d, bitnum %d \n", irqNum, bitNum);
#endif
  mask = ~ (1 << bitNum);
  GT_GPPirq_cache &= mask;
  outl(GT_GPPirq_cache, GT_GPP_Interrupt_Mask);
  rtems_interrupt_enable(level);
}

/* Only print ten entries for now */
void BSP_printMainIrqTbl()
{
  int i;

  printk("mainIrqTbl[10]={");
  for (i=0; i<10; i++)
    printk("%d,", mainIrqTbl[i]);
  printk("}\n");
  printk("GPPinMainIrqTbl 0x%x 0x%x 0x%x 0x%x\n",
	 GPPinMainIrqTbl[0], GPPinMainIrqTbl[1],
	 GPPinMainIrqTbl[2], GPPinMainIrqTbl[3]);
}
